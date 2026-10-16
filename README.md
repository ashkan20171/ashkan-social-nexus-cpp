# Ashkan Social Nexus — Modern C++ Social Platform Prototype

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue) ![CMake](https://img.shields.io/badge/Build-CMake-informational) ![SQLite](https://img.shields.io/badge/Database-SQLite-green) ![Status](https://img.shields.io/badge/Status-Portfolio%20Prototype-orange)

**A full-stack C++ portfolio project exploring social networking, persistent storage, secure credential handling, content moderation, and cross-platform build engineering.**

## Features

- Registration and login with PBKDF2-HMAC-SHA256 password hashing
- Posts, image posts (size-limited), likes, comments, bookmarks and search
- Public profiles, follow/unfollow and media gallery
- Private messages, conversation inbox and read receipts (polling, not WebSocket)
- Blocking, reporting and moderation dashboard with role-based endpoints
- Account password changes, session revocation, bilingual UI elements (EN/FA)
- SQLite migrations, security validation tests, GitHub Actions workflow

## Technology

C++17 · CMake · SQLite3 · OpenSSL · HTML/CSS/JavaScript · CTest

## Build — Windows (Visual Studio / Ninja / vcpkg)

Install Git, CMake, a C++ toolchain, Ninja and [vcpkg](https://github.com/microsoft/vcpkg). Install `sqlite3:x64-windows` and `openssl:x64-windows` and set `VCPKG_ROOT` to your vcpkg folder.

```powershell
cmake --preset windows-vcpkg
cmake --build out/build/windows-vcpkg
ctest --test-dir out/build/windows-vcpkg --output-on-failure
```

Open the project folder in Visual Studio and select the `windows-vcpkg` preset. The executable starts a local HTTP server at **http://127.0.0.1:8080**. On Linux, install the development packages for SQLite3 and OpenSSL and run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/AshkanSocialNexus
```

## Engineering notes

- `src/database.hpp` now owns repeatable schema initialization and migration steps; `src/main.cpp` still contains routing and CRUD logic. Full Controller/Service/Repository separation remains future work.
- `src/api_contract.hpp` centralizes basic method/version/API-path checks; `src/app_logger.hpp` provides UTC event logging without request credentials.
- `tests/` includes security, HTTP and API contract validation tests. Database schema migration tests are also included. These are **unit tests, not comprehensive end-to-end API tests**.
- The application is **local-only** and binds to loopback. It is not production-ready: the minimal HTTP parser, hand-written JSON handling, admin bootstrap behavior, upload validation, session model, and concurrency need a security review and redesign before internet deployment.
- Existing database file `ashkan_social.db` should be backed up before upgrades.

## Roadmap

1. Extract repository, domain services and controllers into independently tested modules.
2. Replace handwritten HTTP/JSON processing with maintained libraries and standardized error responses.
3. Add integration tests, structured logging, pagination and reliable migration management.
4. Introduce production-grade media storage, WebSocket messaging and hardened auth.
5. Improve full Persian RTL/English LTR localization and accessibility.

## Recruiter notes

This project demonstrates incremental delivery, C++ build tooling, database-backed APIs, testing, and security-aware engineering. It is intentionally described as a **prototype**, with architectural and operational limitations documented transparently.

**Suggested GitHub repository:** `ashkan-social-nexus-cpp`  
**GitHub About:** `C++17 social networking prototype | SQLite, OpenSSL, CMake, messaging, moderation, media gallery, CTest & cross-platform CI.`

## License

No license has been selected yet. Add an appropriate license before accepting contributions.

## v16 engineering improvements

- Extracted schema initialization from the HTTP server into `nexus::storage::initialize(sqlite3*)`.
- Added checked SQL execution and transaction rollback on migration failures.
- Made schema migrations repeatable, including handling already-present columns.
- Added an in-memory SQLite migration test, including repeated initialization.
- Note: migrations are not yet versioned and must be reviewed before deployment; the application remains a local portfolio prototype.


## Version 17 engineering updates

- Extracted API routing/handlers from `main.cpp` into `src/api_service.hpp` and socket transport into `src/http_server.hpp`. These are transitional modules, **not** a complete Controller/Service/Repository architecture.
- Added SQLite audit events for role changes and report-status transitions using idempotent database triggers. These records contain the subject and before/after values, but do not yet identify the actor who initiated the change.
- Added `audit_triggers` CTest coverage for repeatable migration and role-change audit.
- Build with CMake and run `ctest --test-dir build --output-on-failure`. Windows builds require vcpkg for SQLite3 and OpenSSL.

**Security warning:** Intended for local development only. Do not expose port 8080 to the public Internet. The minimal HTTP stack and hand-written JSON parsing still require replacement/hardening, and initial administrator bootstrap should be redesigned before deployment.
"# ashkan-social-nexus-cpp" 
