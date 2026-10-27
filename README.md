# Gatehouse VMS — C++ backend + web frontend

A local Visitor Management System built around a C++17 backend. The browser UI talks to a small Node.js HTTP server, which invokes the C++ backend for record operations. Visitor and watchlist data persist in text files in `data/`.

## Requirements
- Windows: MinGW-w64/G++ (C++17) and Node.js LTS.
- macOS/Linux: G++ and Node.js.
- No npm packages are required.

## Run on Windows
1. Extract the whole folder.
2. Open the folder in VS Code.
3. Double-click `start.bat`, or run it from the VS Code terminal.
4. Open `http://localhost:3000` if it does not open automatically.
5. Keep the terminal window open while using the website.

## Run on macOS/Linux
Run `./start.sh`, then open `http://localhost:3000`.

## Features
- Dashboard statistics and recent activity
- Visitor registration with automatic watchlist screening
- Pending approval queue, approve/reject, check-in and check-out
- Search and filter visitor records
- Watchlist add/remove
- CSV export
- Persistent records in `data/visitors.txt` and `data/blacklist.txt`

## Important project notes
- Approval notifications and digital pass strings are simulated for a lab demonstration; no real SMS/email is sent and no scannable QR image is generated.
- Admin-only access to the watchlist is not yet authenticated in this local demo. Do not expose this server to the public internet.
- This is a local development project, not a production security system.

## OOP viva points
- `Visitor` is a data model that groups visitor attributes.
- Functions separate loading/saving, blacklist screening, registration and status transitions.
- Encapsulation means internal data is manipulated through backend operations rather than directly by the webpage.
- The current implementation does not force inheritance/polymorphism where they do not add value. If your lab rubric requires those explicitly, add them meaningfully and explain the use case.
