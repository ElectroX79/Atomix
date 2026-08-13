# Development Workflow

The Atomix development workflow is simple. The amount of work needed depends on the size and impact of the change.

## 1. Choose what to work on

Check the existing issues and documentation before starting.

For large or important changes, discuss the idea in an issue first.

Small fixes can usually be started directly.

## 2. Create a branch

Create a new branch for your change.

Use a clear name, for example:

* `feature/data-table-extract`
* `fix/buffer-bounds`
* `refactor/mem-route`

Do not work directly on `main`.

## 3. Implement and test
Make your changes and add or update tests when needed, also run the tests before opening a pull request.


Also consider if your code changes need additional commits:
- For changes that may affect performance, create or run the **relevant** benchmarks.
- Update the documentation when **necessary**.
- If the change affects the architecture, check if an ADR is needed.

## 4. Open a Pull Request

Create a pull request when your change is ready.

Explain:

* what you changed;
* why you changed it;
* how you tested it;
* whether you ran benchmarks.


## 5. Review and merge

Other contributors will review the pull request, sometimes they will suggest changes or ask questions.
Discuss them openly and make the necessary changes.


## General rule

The amount of validation should depend on the impact of the change.  For example, a documentation fix does not need the same process as a change to the memory system.

