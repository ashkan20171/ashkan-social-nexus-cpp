#pragma once
#include <sqlite3.h>
#include <stdexcept>
#include <string>
namespace nexus::storage {
inline void exec(sqlite3* db,const char* statement,bool optionalDuplicate=false){
 char* err=nullptr; int rc=sqlite3_exec(db,statement,nullptr,nullptr,&err);
 std::string message=err?err:"SQLite operation failed"; sqlite3_free(err);
 if(rc!=SQLITE_OK && !(optionalDuplicate && message.find("duplicate column name")!=std::string::npos)) throw std::runtime_error(message);
}
inline void initialize(sqlite3* db){
 exec(db,"PRAGMA foreign_keys=ON;");
 exec(db,"PRAGMA journal_mode=WAL;");
 exec(db,"BEGIN IMMEDIATE;");
 try {
  exec(db,"CREATE TABLE IF NOT EXISTS users(id INTEGER PRIMARY KEY,username TEXT UNIQUE NOT NULL,salt TEXT NOT NULL,passhash TEXT NOT NULL);");
  exec(db,"ALTER TABLE users ADD COLUMN role TEXT NOT NULL DEFAULT 'member'",true);
  exec(db,"UPDATE users SET role='admin' WHERE id=(SELECT MIN(id) FROM users) AND NOT EXISTS(SELECT 1 FROM users WHERE role='admin')");
  exec(db,"CREATE TABLE IF NOT EXISTS sessions(token TEXT PRIMARY KEY,user_id INTEGER REFERENCES users(id),created_at TEXT DEFAULT CURRENT_TIMESTAMP);");
  exec(db,"CREATE TABLE IF NOT EXISTS posts(id INTEGER PRIMARY KEY,user_id INTEGER REFERENCES users(id),content TEXT NOT NULL,created_at TEXT DEFAULT CURRENT_TIMESTAMP);");
  exec(db,"ALTER TABLE posts ADD COLUMN image_data TEXT DEFAULT ''",true);
  exec(db,"CREATE TABLE IF NOT EXISTS likes(post_id INTEGER REFERENCES posts(id),user_id INTEGER REFERENCES users(id),PRIMARY KEY(post_id,user_id));");
  exec(db,"CREATE TABLE IF NOT EXISTS comments(id INTEGER PRIMARY KEY,post_id INTEGER NOT NULL REFERENCES posts(id),user_id INTEGER NOT NULL REFERENCES users(id),content TEXT NOT NULL,created_at TEXT DEFAULT CURRENT_TIMESTAMP);");
  exec(db,"CREATE TABLE IF NOT EXISTS messages(id INTEGER PRIMARY KEY,sender TEXT,recipient TEXT,body TEXT,created_at TEXT DEFAULT CURRENT_TIMESTAMP);");
  exec(db,"CREATE TABLE IF NOT EXISTS follows(follower_id INTEGER NOT NULL REFERENCES users(id),followed_id INTEGER NOT NULL REFERENCES users(id),PRIMARY KEY(follower_id,followed_id));");
  exec(db,"CREATE TABLE IF NOT EXISTS bookmarks(user_id INTEGER NOT NULL REFERENCES users(id),post_id INTEGER NOT NULL REFERENCES posts(id),created_at TEXT DEFAULT CURRENT_TIMESTAMP,PRIMARY KEY(user_id,post_id));");
  exec(db,"CREATE TABLE IF NOT EXISTS blocks(blocker_id INTEGER NOT NULL REFERENCES users(id),blocked_id INTEGER NOT NULL REFERENCES users(id),PRIMARY KEY(blocker_id,blocked_id));");
  exec(db,"CREATE TABLE IF NOT EXISTS reports(id INTEGER PRIMARY KEY,reporter_id INTEGER NOT NULL REFERENCES users(id),post_id INTEGER NOT NULL REFERENCES posts(id),reason TEXT NOT NULL,status TEXT NOT NULL DEFAULT 'pending',created_at TEXT DEFAULT CURRENT_TIMESTAMP);");
  exec(db,"CREATE TABLE IF NOT EXISTS notifications(id INTEGER PRIMARY KEY,recipient TEXT NOT NULL,actor TEXT NOT NULL,kind TEXT NOT NULL,created_at TEXT DEFAULT CURRENT_TIMESTAMP);");
  exec(db,"ALTER TABLE users ADD COLUMN bio TEXT DEFAULT ''",true);
  exec(db,"ALTER TABLE users ADD COLUMN display_name TEXT DEFAULT ''",true);
  exec(db,"CREATE INDEX IF NOT EXISTS idx_messages_sender_id ON messages(sender,id);");
  exec(db,"CREATE INDEX IF NOT EXISTS idx_messages_recipient_id ON messages(recipient,id);");
  exec(db,"ALTER TABLE messages ADD COLUMN read_at TEXT",true);
  exec(db,"CREATE TABLE IF NOT EXISTS audit_events(id INTEGER PRIMARY KEY AUTOINCREMENT, event_type TEXT NOT NULL, subject_id INTEGER NOT NULL, old_value TEXT, new_value TEXT, created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP);");
  exec(db,"CREATE INDEX IF NOT EXISTS idx_audit_events_created ON audit_events(id DESC);");
  exec(db,"CREATE TRIGGER IF NOT EXISTS audit_user_role AFTER UPDATE OF role ON users WHEN OLD.role <> NEW.role BEGIN INSERT INTO audit_events(event_type,subject_id,old_value,new_value) VALUES('user.role',NEW.id,OLD.role,NEW.role); END;");
  exec(db,"CREATE TRIGGER IF NOT EXISTS audit_report_status AFTER UPDATE OF status ON reports WHEN OLD.status <> NEW.status BEGIN INSERT INTO audit_events(event_type,subject_id,old_value,new_value) VALUES('report.status',NEW.id,OLD.status,NEW.status); END;");
  exec(db,"COMMIT;");
 } catch (...) { sqlite3_exec(db,"ROLLBACK;",nullptr,nullptr,nullptr); throw; }
}
}
