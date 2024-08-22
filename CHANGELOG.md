# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixes

- Memory leak in `TROC_deinitRegistry`.
- Test failure reporting in non-color mode.
- Private declarations leaking into public API.

## [0.0.1] - 2024-08-22

### Added

- `TROC_test` macro to define tests.
- Assert macros:
  - `TROC_expect`: make an assertion during a test.
  - `TROC_assert`: stop the test if an assertion fails.
  - `TROC_failMsg`: fail the test with a custom message.
- `TROC_runTests` to run all tests.
- `TROC_finish` to print test results and cleanup.

## [0.0.0] - 2024-08-21

### Added

- Project structure.
- Zig build system.
- Make build system.

[Unreleased]: https://github.com/sonro/trocious/compare/v0.0.1...HEAD
[0.0.1]: https://github.com/sonro/trocious/releases/tag/v0.0.1
[0.0.0]: https://github.com/sonro/trocious/releases/tag/v0.0.0
