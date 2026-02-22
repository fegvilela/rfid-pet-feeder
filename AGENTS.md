# AGENTS.md

Guidelines for coding agents working on the Fedorentos Pet Feeder project.

## Project Overview

Automated pet feeder system using Raspberry Pi, Viam SDK, image recognition, and servo motors. Controls 3 feeders with individual access permissions for 5 cats.

## Build/Lint/Test Commands

This project uses `uv` for dependency management.

### Setup

```bash
uv sync
```

### Run the application

```bash
uv run python -m src.main
```

### Type checking

```bash
uv run mypy src/
```

### Linting

```bash
uv run ruff check src/
uv run ruff format src/ --check
```

### Running tests

```bash
uv run pytest tests/ -v
```

### Running a single test

```bash
uv run pytest tests/path/to/test_file.py::test_function_name -v
uv run pytest tests/test_feeder.py::test_open_for_cat -v
```

## Code Style Guidelines

### Imports

Order imports in three groups, separated by blank lines:
1. Standard library imports (alphabetical)
2. Third-party imports (alphabetical)
3. Local imports (alphabetical)

```python
import asyncio
import os
from datetime import datetime

from viam.components.board import Board
from viam.services.vision import VisionClient

from src.config import CONFIDENCE_THRESHOLD
from src.utils.logger import logger
```

### Type Hints

- Use Python 3.10+ union syntax: `datetime | None` instead of `Optional[datetime]`
- Use lowercase generics: `list[str]`, `dict[int, Feeder]` instead of `List[str]`, `Dict[int, Feeder]`
- Always annotate function parameters and return types
- Use `-> None` for functions that don't return a value

```python
def __init__(self, feeder_id: int, allowed_cats: list[str]):
    ...

async def classify(self) -> tuple[str | None, float]:
    ...

async def close_all(self) -> None:
    ...
```

### Naming Conventions

- **Classes**: PascalCase (`Feeder`, `FeederManager`, `ServoController`)
- **Functions/Methods**: snake_case (`open_for_cat`, `handle_cat_detected`)
- **Private methods**: Prefix with underscore (`_build_feeders`, `_set_angle`, `_safe_close`)
- **Variables**: snake_case (`feeder_id`, `cat_name`, `is_busy`)
- **Constants**: UPPER_SNAKE_CASE (`SERVO_PINS`, `FEEDER_OPEN_TIMEOUT`)
- **Instance attributes**: snake_case, boolean attributes prefix with `is_`, `has_`, etc. (`is_open`, `is_busy`)

### Classes

- Use `__init__` for initialization with clear parameter typing
- Group related methods together
- Place public methods before private methods

```python
class Feeder:
    def __init__(self, feeder_id: int, servo: ServoController, sensor: PresenceSensor, allowed_cats: list[str]):
        self.feeder_id = feeder_id
        self.servo = servo
        self.sensor = sensor
        self.allowed_cats = allowed_cats
        self.is_busy = False
    
    def can_access(self, cat_name: str) -> bool:
        ...
    
    async def open_for_cat(self, cat_name: str) -> bool:
        ...
    
    async def _wait_and_close(self) -> None:
        ...
```

### Async/Await

- All hardware operations must be async
- Use `asyncio.create_task()` for concurrent operations
- Handle `asyncio.CancelledError` in long-running tasks

```python
async def monitor_and_close(self) -> None:
    try:
        await self._wait_and_close()
    except asyncio.CancelledError:
        logger.info("Monitoring cancelled")
    finally:
        await self._safe_close()
```

### Error Handling

- Use try/except for operations that can fail
- Log errors using the project logger
- Return safe default values on error

```python
async def classify(self) -> tuple[str | None, float]:
    try:
        classifications = await self.vision.get_classifications_from_camera(...)
        if not classifications:
            return None, 0.0
        return classifications[0].class_name, classifications[0].confidence
    except Exception as e:
        logger.error(f"Error during classification: {e}")
        return None, 0.0
```

### Logging

- Use the project logger from `src.utils.logger`
- Log levels: `logger.debug()`, `logger.info()`, `logger.warning()`, `logger.error()`
- Include context in log messages (feeder_id, cat_name, etc.)

```python
from src.utils.logger import logger

logger.info(f"Feeder {self.feeder_id}: Opening for cat '{cat_name}'")
logger.warning(f"Feeder {self.feeder_id}: Timeout reached, closing")
logger.error(f"Feeder {self.feeder_id}: Error closing servo: {e}")
```

### Comments

- Do NOT add comments unless explicitly requested
- Code should be self-documenting through clear naming
- Docstrings only for public APIs if requested

### Configuration

- All configuration via environment variables in `src/config.py`
- Copy `.env.example` to `.env` and fill in values
- Never commit secrets or `.env` files

### File Structure

```
src/
├── main.py              # Entry point, application lifecycle
├── config.py            # Environment configuration constants
├── viam_connection.py   # Viam SDK connection management
├── hardware/            # Hardware abstractions
│   ├── servo_controller.py
│   └── presence_sensor.py
├── vision/              # Image recognition
│   └── classifier.py
├── feeder/              # Business logic
│   ├── feeder.py
│   └── feeder_manager.py
└── utils/               # Utilities
    └── logger.py
```

### Testing

- Place tests in `tests/` directory mirroring `src/` structure
- Use pytest with `pytest-asyncio` for async tests
- Name test files `test_<module_name>.py`
- Name test functions `test_<function_name>`

```python
# tests/test_feeder.py
import pytest
from src.feeder.feeder import Feeder

@pytest.mark.asyncio
async def test_open_for_cat():
    ...

def test_can_access():
    ...
```

## Hardware Notes

- Servos use PWM on GPIO pins (default: 18, 19, 20)
- Presence sensors use GPIO digital read (default: 23, 24, 25)
- Always use async methods for hardware operations
- External power supply required for servos (not GPIO power)
