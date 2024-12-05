-- yes
PRAGMA application_id = 0x14585583;

-- enable foreign keys, just in case
PRAGMA foreign_keys = ON;

-- enable write-ahead logging
PRAGMA journal_mode=WAL;

CREATE TABLE todo (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    completed BOOLEAN NOT NULL,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);
