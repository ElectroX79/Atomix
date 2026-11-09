## Architecture Decision Records

An Architecture Decision Record (ADR) documents an important architectural decision.

Create an ADR when a change:

* introduces an important architectural decision;
* changes an existing architectural decision;
* has important trade-offs;
* or may be difficult to understand in the future without its context.

Small implementation and most of the changes do not need an ADR.

### Creating an ADR

Create a new file in:

```text
docs/adr/
```

Use the next available number:

```text
adr-0004-short-description.md
```

Keep the ADR short and focused.

An ADR should explain:

* **Status** — the current status of the decision. *(Notice that an ADR could be a proposal)*
* **Context** — the problem or situation that led to the decision.
* **Decision** — what was decided.
* **Motivation** — why the decision was made.
* **Consequences** — the main advantages and disadvantages.

Example:

```markdown
# Shared ownership for the `Buffer` class (ADR-0001)

## Status

Accepted

## Context

The `Column` class previously stored a `Buffer` directly.

## Decision

A `Column` now stores a `shared_ptr` to `Buffer`.

## Motivation

- Enable shallow copies instead of deep copies.
- Simplify memory ownership.

## Consequences

### Advantages

- Potential Copy-on-Write.
- Shallow copies.
- Simpler memory ownership.

### Disadvantages

- Additional metadata.
- Atomic reference-count operations.
```

An ADR should describe **why a decision was made**, not explain the complete implementation.
