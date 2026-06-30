/*
 Navicat Premium Data Transfer

 Source Server         : iptv_def
 Source Server Type    : SQLite
 Source Server Version : 3035005 (3.35.5)
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3035005 (3.35.5)
 File Encoding         : 65001

 Date: 02/07/2025 12:21:17
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for source_type
-- ----------------------------
DROP TABLE IF EXISTS "source_type";
CREATE TABLE "source_type" (
  "id" int NOT NULL,
  "name" varchar(255) NOT NULL,
  PRIMARY KEY ("id")
);

-- ----------------------------
-- Records of source_type
-- ----------------------------
INSERT INTO "source_type" VALUES (0, 'TS');
INSERT INTO "source_type" VALUES (1, 'SD');
INSERT INTO "source_type" VALUES (2, 'HD');
INSERT INTO "source_type" VALUES (3, '4K');
INSERT INTO "source_type" VALUES (4, '8K');

-- ----------------------------
-- Table structure for tv_info
-- ----------------------------
DROP TABLE IF EXISTS "tv_info";
CREATE TABLE "tv_info" (
  "id" int NOT NULL,
  "channel_id" int DEFAULT NULL,
  "name" varchar(255) NOT NULL,
  "group" varchar(255) DEFAULT NULL,
  "city" varchar(255) DEFAULT NULL,
  "describe" text,
  "notes" text,
  "logo_name" varchar(255) DEFAULT NULL,
  PRIMARY KEY ("id"),
  UNIQUE ("name" ASC)
);

-- ----------------------------
-- Records of tv_info
-- ----------------------------

-- ----------------------------
-- Table structure for tv_source_info
-- ----------------------------
DROP TABLE IF EXISTS "tv_source_info";
CREATE TABLE "tv_source_info" (
  "id" int NOT NULL,
  "tv_id" int NOT NULL,
  "ip" varchar(255) DEFAULT NULL,
  "port" int DEFAULT NULL,
  "width" int NOT NULL,
  "height" int NOT NULL,
  "fps" int NOT NULL,
  "last_check_date" datetime NOT NULL,
  "type" int NOT NULL DEFAULT 0,
  "notes" text,
  PRIMARY KEY ("id"),
  CONSTRAINT "tv_source_info_ibfk_1" FOREIGN KEY ("tv_id") REFERENCES "tv_info" ("id") ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT "tv_source_info_ibfk_2" FOREIGN KEY ("type") REFERENCES "source_type" ("id") ON DELETE RESTRICT ON UPDATE RESTRICT,
  UNIQUE ("ip" ASC)
);

-- ----------------------------
-- Records of tv_source_info
-- ----------------------------

PRAGMA foreign_keys = true;
