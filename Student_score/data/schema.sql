BEGIN TRANSACTION;

-- 1. Tắt khóa ngoại để xóa bảng cũ an toàn
PRAGMA foreign_keys = OFF;

-- Xóa bảng cũ nếu có (Clean slate)
DROP TABLE IF EXISTS grades;
DROP TABLE IF EXISTS exams;
DROP TABLE IF EXISTS terms;
DROP TABLE IF EXISTS subjects;
DROP TABLE IF EXISTS students;
DROP TABLE IF EXISTS classes;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS 成绩表;
DROP TABLE IF EXISTS 学生扩展表;
DROP TABLE IF EXISTS 班级表;
DROP TABLE IF EXISTS 考试类型表;
DROP TABLE IF EXISTS 学期表;
DROP TABLE IF EXISTS 科目表;
DROP TABLE IF EXISTS 用户表;

-- Reset bộ đếm ID
DELETE FROM sqlite_sequence;

-- Bật lại khóa ngoại
PRAGMA foreign_keys = ON;

-- 2. TẠO CẤU TRÚC BẢNG MỚI (Schema Design)

-- Bảng 1: Users
CREATE TABLE 用户表 (
    用户ID INTEGER PRIMARY KEY AUTOINCREMENT,
    用户名 TEXT NOT NULL UNIQUE, -- Sẽ là Student ID (23xxxxx)
    密码哈希 TEXT NOT NULL,        -- Sẽ là 3 số cuối ID
    角色 INTEGER NOT NULL,         -- 1: Admin, 2: Teacher, 3: Student
    真实姓名 TEXT NOT NULL,
    性别 TEXT,
    状态 INTEGER DEFAULT 1,
    创建时间 TEXT DEFAULT (datetime('now', 'localtime'))
);

-- Bảng 2: Classes
CREATE TABLE 班级表 (
    班级ID INTEGER PRIMARY KEY AUTOINCREMENT,
    班级名称 TEXT NOT NULL UNIQUE,
    年级 TEXT NOT NULL,
    班主任ID INTEGER,
    FOREIGN KEY(班主任ID) REFERENCES 用户表(用户ID)
);

-- Bảng 3: Students (Thông tin mở rộng)
CREATE TABLE 学生扩展表 (
    学生ID INTEGER PRIMARY KEY,
    学号 TEXT UNIQUE NOT NULL,     -- ID sinh viên chuẩn (2300001...)
    班级ID INTEGER NOT NULL,
    FOREIGN KEY(学生ID) REFERENCES 用户表(用户ID) ON DELETE CASCADE,
    FOREIGN KEY(班级ID) REFERENCES 班级表(班级ID) ON DELETE CASCADE
);

-- Bảng 4: Subjects
CREATE TABLE 科目表 (
    科目ID INTEGER PRIMARY KEY AUTOINCREMENT,
    科目名称 TEXT NOT NULL UNIQUE,
    满分 REAL DEFAULT 100
);

-- Bảng 5: Terms
CREATE TABLE 学期表 (
    学期ID INTEGER PRIMARY KEY AUTOINCREMENT,
    学期名称 TEXT NOT NULL UNIQUE
);

-- Bảng 6: Exams
CREATE TABLE 考试类型表 (
    考试ID INTEGER PRIMARY KEY AUTOINCREMENT,
    考试名称 TEXT NOT NULL,
    学期ID INTEGER NOT NULL,
    考试日期 TEXT NOT NULL,
    FOREIGN KEY(学期ID) REFERENCES 学期表(学期ID)
);

-- Bảng 7: Grades
CREATE TABLE 成绩表 (
    成绩ID INTEGER PRIMARY KEY AUTOINCREMENT,
    学生ID INTEGER NOT NULL,
    科目ID INTEGER NOT NULL,
    考试ID INTEGER NOT NULL,
    分数 REAL NOT NULL CHECK(分数 >= 0),
    录入人 INTEGER NOT NULL,
    录入时间 TEXT DEFAULT (datetime('now', 'localtime')),
    状态 INTEGER DEFAULT 1,
    UNIQUE(学生ID, 科目ID, 考试ID),
    FOREIGN KEY(学生ID) REFERENCES 用户表(用户ID) ON DELETE CASCADE,
    FOREIGN KEY(科目ID) REFERENCES 科目表(科目ID),
    FOREIGN KEY(考试ID) REFERENCES 考试类型表(考试ID),
    FOREIGN KEY(录入人) REFERENCES 用户表(用户ID)
);

-- 3. DỮ LIỆU MẪU CƠ BẢN

-- 3.1. Admin & Môn học
INSERT INTO 用户表 (用户名, 密码哈希, 角色, 真实姓名) VALUES ('admin', 'admin123', 1, 'Administrator');

INSERT INTO 科目表 (科目名称, 满分) VALUES
('数学', 100), ('物理', 100), ('化学', 100), ('英语', 100),
('语文', 100), ('生物', 100), ('历史', 100), ('地理', 100);

-- 3.2. Lớp học (9 Lớp: 3 Khối x 3 Lớp)
INSERT INTO 班级表 (班级名称, 年级) VALUES
('高一A班', '10'), ('高一B班', '10'), ('高一C班', '10'),
('高二A班', '11'), ('高二B班', '11'), ('高二C班', '11'),
('高三A班', '12'), ('高三B班', '12'), ('高三C班', '12');

-- 3.3. Học kỳ & Kỳ thi
INSERT INTO 学期表 (学期名称) VALUES ('2024-2025 第一学期');
INSERT INTO 考试类型表 (考试名称, 考试日期, 学期ID) 
SELECT '期中考试', '2024-11-10', 学期ID FROM 学期表 WHERE 学期名称 = '2024-2025 第一学期' LIMIT 1;


-- 4. SINH 360 HỌC SINH (LOGIC PHỨC TẠP)

-- Sử dụng CTE để tạo tên ngẫu nhiên không trùng lặp
WITH RECURSIVE 
    -- Sinh 5000 số để làm nguồn random
    generate_series(x) AS (
        SELECT 1 UNION ALL SELECT x+1 FROM generate_series WHERE x < 5000
    ),
    -- Tạo kho tên
    raw_names AS (
        SELECT 
            x,
            CASE (ABS(RANDOM()) % 2) WHEN 0 THEN '男' ELSE '女' END as gender_val,
            -- 100 Họ phổ biến
            CASE (ABS(RANDOM()) % 60)
                WHEN 0 THEN '李' WHEN 1 THEN '王' WHEN 2 THEN '张' WHEN 3 THEN '刘' WHEN 4 THEN '陈'
                WHEN 5 THEN '杨' WHEN 6 THEN '赵' WHEN 7 THEN '黄' WHEN 8 THEN '周' WHEN 9 THEN '吴'
                WHEN 10 THEN '徐' WHEN 11 THEN '孙' WHEN 12 THEN '胡' WHEN 13 THEN '朱' WHEN 14 THEN '高'
                WHEN 15 THEN '林' WHEN 16 THEN '何' WHEN 17 THEN '郭' WHEN 18 THEN '马' WHEN 19 THEN '罗'
                WHEN 20 THEN '梁' WHEN 21 THEN '宋' WHEN 22 THEN '郑' WHEN 23 THEN '谢' WHEN 24 THEN '韩'
                WHEN 25 THEN '唐' WHEN 26 THEN '冯' WHEN 27 THEN '于' WHEN 28 THEN '董' WHEN 29 THEN '萧'
                WHEN 30 THEN '程' WHEN 31 THEN '曹' WHEN 32 THEN '袁' WHEN 33 THEN '邓' WHEN 34 THEN '许'
                WHEN 35 THEN '傅' WHEN 36 THEN '沈' WHEN 37 THEN '曾' WHEN 38 THEN '彭' WHEN 39 THEN '吕'
                WHEN 40 THEN '苏' WHEN 41 THEN '卢' WHEN 42 THEN '蒋' WHEN 43 THEN '蔡' WHEN 44 THEN '贾'
                WHEN 45 THEN '丁' WHEN 46 THEN '魏' WHEN 47 THEN '薛' WHEN 48 THEN '叶' WHEN 49 THEN '阎'
                WHEN 50 THEN '余' WHEN 51 THEN '潘' WHEN 52 THEN '杜' WHEN 53 THEN '戴' WHEN 54 THEN '夏'
                WHEN 55 THEN '钟' WHEN 56 THEN '汪' WHEN 57 THEN '田' WHEN 58 THEN '任' ELSE '姜'
            END as surname,
            -- Tên đệm phong phú
            CASE (ABS(RANDOM()) % 40)
                WHEN 0 THEN '文' WHEN 1 THEN '小' WHEN 2 THEN '志' WHEN 3 THEN '建' WHEN 4 THEN '家'
                WHEN 5 THEN '晓' WHEN 6 THEN '天' WHEN 7 THEN '立' WHEN 8 THEN '佳' WHEN 9 THEN '宏'
                WHEN 10 THEN '子' WHEN 11 THEN '梓' WHEN 12 THEN '一' WHEN 13 THEN '博' WHEN 14 THEN '思'
                WHEN 15 THEN '若' WHEN 16 THEN '永' WHEN 17 THEN '春' WHEN 18 THEN '宗' WHEN 19 THEN '雨'
                WHEN 20 THEN '晨' WHEN 21 THEN '艺' WHEN 22 THEN '书' WHEN 23 THEN '嘉' WHEN 24 THEN '梦'
                WHEN 25 THEN '楚' WHEN 26 THEN '润' WHEN 27 THEN '泽' WHEN 28 THEN '景' WHEN 29 THEN '亦'
                WHEN 30 THEN '承' WHEN 31 THEN '智' WHEN 32 THEN '世' WHEN 33 THEN '正' WHEN 34 THEN '佑'
                WHEN 35 THEN '俊' WHEN 36 THEN '彦' WHEN 37 THEN '柏' WHEN 38 THEN '仲' ELSE ''
            END as middle,
            -- Tên chính (Nam/Nữ)
            CASE WHEN (ABS(RANDOM()) % 2) = 0 THEN 
                CASE (ABS(RANDOM()) % 30)
                    WHEN 0 THEN '伟' WHEN 1 THEN '强' WHEN 2 THEN '磊' WHEN 3 THEN '洋' WHEN 4 THEN '勇'
                    WHEN 5 THEN '军' WHEN 6 THEN '杰' WHEN 7 THEN '涛' WHEN 8 THEN '明' WHEN 9 THEN '超'
                    WHEN 10 THEN '秀' WHEN 11 THEN '浩' WHEN 12 THEN '刚' WHEN 13 THEN '平' WHEN 14 THEN '辉'
                    WHEN 15 THEN '鹏' WHEN 16 THEN '华' WHEN 17 THEN '飞' WHEN 18 THEN '鑫' WHEN 19 THEN '波'
                    WHEN 20 THEN '斌' WHEN 21 THEN '凯' WHEN 22 THEN '翔' WHEN 23 THEN '旭' WHEN 24 THEN '然'
                    WHEN 25 THEN '成' WHEN 26 THEN '康' WHEN 27 THEN '峰' WHEN 28 THEN '帅' ELSE '龙'
                END
            ELSE 
                CASE (ABS(RANDOM()) % 30)
                    WHEN 0 THEN '静' WHEN 1 THEN '丽' WHEN 2 THEN '娟' WHEN 3 THEN '敏' WHEN 4 THEN '燕'
                    WHEN 5 THEN '艳' WHEN 6 THEN '兰' WHEN 7 THEN '娜' WHEN 8 THEN '琳' WHEN 9 THEN '洁'
                    WHEN 10 THEN '梅' WHEN 11 THEN '菲' WHEN 12 THEN '雪' WHEN 13 THEN '婷' WHEN 14 THEN '慧'
                    WHEN 15 THEN '莹' WHEN 16 THEN '玉' WHEN 17 THEN '萍' WHEN 18 THEN '红' WHEN 19 THEN '玲'
                    WHEN 20 THEN '霞' WHEN 21 THEN '月' WHEN 22 THEN '彩' WHEN 23 THEN '怡' WHEN 24 THEN '倩'
                    WHEN 25 THEN '丹' WHEN 26 THEN '薇' WHEN 27 THEN '琪' WHEN 28 THEN '珊' ELSE '瑶'
                END
            END as firstname
        FROM generate_series
    )
-- Insert User tạm thời (sẽ update Username sau)
INSERT INTO 用户表 (用户名, 密码哈希, 角色, 真实姓名, 性别)
SELECT DISTINCT
    'temp_' || x, 'temp_pass', 3,
    surname || middle || firstname,
    gender_val
FROM raw_names
GROUP BY surname || middle || firstname
LIMIT 360;

-- 5. PHÂN LỚP & SINH STUDENT ID CHUẨN
-- Logic: Năm nhập học (2 số) + 00000 + Số thứ tự trong khối
-- Năm nay 2025 -> Lớp 10 (25), Lớp 11 (24), Lớp 12 (23)

INSERT INTO 学生扩展表 (学生ID, 班级ID, 学号)
SELECT 
    u.用户ID,
    c.班级ID,
    -- Tạo ID: [Năm] + [00000 + STT] (lấy 5 số cuối)
    (CASE c.年级 
        WHEN '10' THEN '25' 
        WHEN '11' THEN '24' 
        ELSE '23' 
    END) || printf('%05d', ROW_NUMBER() OVER (PARTITION BY c.年级 ORDER BY u.用户ID))
FROM 用户表 u
JOIN (
    -- Chia đều 360 học sinh vào 9 lớp (mỗi lớp 40 em)
    SELECT 班级ID, 年级, ROW_NUMBER() OVER (ORDER BY 班级ID) - 1 as class_idx
    FROM 班级表
) c ON c.class_idx = ((u.用户ID - (SELECT MIN(用户ID) FROM 用户表 WHERE 角色=3)) % 9)
WHERE u.角色 = 3;

-- 6. CẬP NHẬT USERNAME VÀ PASSWORD
-- Username = Student ID
-- Password = 3 số cuối của ID
UPDATE 用户表
SET 
    用户名 = (SELECT 学号 FROM 学生扩展表 WHERE 学生ID = 用户表.用户ID),
    密码哈希 = SUBSTR((SELECT 学号 FROM 学生扩展表 WHERE 学生ID = 用户表.用户ID), -3)
WHERE 角色 = 3;

-- 7. TẠO ĐIỂM SỐ NGẪU NHIÊN
INSERT INTO 成绩表 (学生ID, 科目ID, 考试ID, 分数, 录入人, 状态)
SELECT 
    u.用户ID, k.科目ID, (SELECT 考试ID FROM 考试类型表 LIMIT 1),
    -- Điểm ngẫu nhiên phân phối thực tế
    CASE 
        WHEN k.科目名称 IN ('数学', '物理', '化学') THEN CAST(ABS(RANDOM() % 61) + 40 AS REAL) -- 40-100
        ELSE CAST(ABS(RANDOM() % 41) + 55 AS REAL) -- 55-95
    END,
    1, 1
FROM 用户表 u
CROSS JOIN 科目表 k
WHERE u.角色 = 3;

COMMIT;