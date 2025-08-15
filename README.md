
[English](#-english-version) | [العربية](#-arabic-version)

<br>

## 🇬🇧 English Version

# 📖 Library Management System

## Overview

This project is a comprehensive, console-based Library Management System built in **C**. It provides a robust command-line interface (CLI) for managing books, members, and the borrowing/returning process. The system is designed to be efficient, secure, and cross-platform compatible (Windows & Linux/Unix).

## ✨ Key Features

### 🌟 General Features
- **Cross-Platform:** Code is compatible with both Windows and Linux/Unix environments.
- **Command-Line Interface (CLI):** An intuitive, menu-driven interface for easy navigation.
- **File-Based Storage:** All data (books, members, transactions) is persistently stored in `.txt` files.
- **Enhanced UI:** Utilizes ANSI color codes to improve readability and user experience.
- **Pagination:** Displays long lists (like books and transactions) across multiple pages for better readability.
- **Security:**
    - **File Locking:** Prevents data corruption during simultaneous write operations.
    - **Password Encryption:** Member passwords are encrypted using a simple Caesar Cipher.
    - **Password Strength Validation:** Enforces different complexity rules for admin and member passwords.
    - **Masked Password Input:** Hides characters during password entry.
    - **Secure Sessions:** Includes a session timeout for inactivity and limits login attempts to prevent brute-force attacks.

### 👨‍💼 Admin Privileges
- Add and delete books from the library catalog.
- Add and delete members from the system.
- View a complete history of all transactions.
- Reset the password for any member account.

### 🧑‍💻 Member Features
- Search for books by title, author, or category.
- Borrow available books.
- Return borrowed books (with automatic fine calculation for overdue items).
- View personal borrowing history.
- Change their own password upon first login.

## 🛠️ Technology Stack
- **Programming Language:** C
- **Key Concepts:**
    - Data Structures (Structs)
    - Dynamic Memory Management (`malloc`, `realloc`)
    - File I/O (`fopen`, `fprintf`, `fscanf`)
    - Cross-platform conditional compilation (`#ifdef _WIN32`)

## 🚀 Getting Started

To compile and run this project, you will need a C compiler like **GCC**.

1.  **Save the Code:**
    Save the provided C code into a file named `library_system.c`.

2.  **Compile:**
    Open your terminal or command prompt, navigate to the directory containing the file, and compile it using the following command:
    ```bash
    gcc library_system.c -o library_system
    ```

3.  **Run:**
    Once compiled successfully, an executable file will be created. Run it with the following command:
    -   On Linux/macOS:
        ```bash
        ./library_system
        ```
    -   On Windows:
        ```bash
        library_system.exe
        ```
    The application will automatically create the necessary data files (`books.txt`, `members.txt`, `transactions.txt`) on its first run.

## 📋 How to Use

When you start the program, you will be presented with a main menu to log in.

1.  **Admin Login:**
    -   **Username:** `admin`
    -   **Default Password:** `Admin@1234`
    -   The admin has full access to all management functions.

2.  **Member Login:**
    -   Member accounts must be created by an admin first.
    -   On their first login, members will be prompted to change their initial password.

3.  **Exit:**
    -   To safely close the application.

## 📁 File Structure
- `library_system.c`: The main source file containing all the application logic.
- `books.txt`: Stores the library's book collection data.
- `members.txt`: Stores member account information, including encrypted passwords.
- `transactions.txt`: Logs all borrow and return activities.

---
<br>

## 🇸🇦 النسخة العربية

# 📖 نظام إدارة المكتبات

## نظرة عامة

هذا المشروع هو نظام إدارة مكتبات متكامل مبني باستخدام لغة البرمجة **C**. يوفر النظام واجهة مستخدم نصية (Console-based) لإدارة الكتب والأعضاء وعمليات الإعارة والإرجاع. تم تصميمه ليكون قوياً، فعالاً، وداعماً لبيئات التشغيل المختلفة (Windows و Linux/Unix).

## ✨ الميزات الرئيسية

### 🌟 ميزات عامة
- **دعم متعدد المنصات:** الكود مكتوب ليعمل على أنظمة Windows و Linux/Unix.
- **واجهة مستخدم نصية (CLI):** واجهة سهلة الاستخدام تعتمد على القوائم والأوامر النصية.
- **تخزين البيانات:** يتم حفظ جميع البيانات (الكتب، الأعضاء، المعاملات) في ملفات نصية (`.txt`).
- **واجهة مستخدم محسّنة:** استخدام أكواد ألوان ANSI لتحسين وضوح الواجهة وتجربة المستخدم.
- **عرض مقسّم للصفحات (Pagination):** عرض القوائم الطويلة (مثل الكتب والمعاملات) على صفحات متعددة لتسهيل التصفح.
- **أمان البيانات:**
    - **قفل الملفات (File Locking):** يمنع تلف البيانات عند محاولة الوصول المتزامن للملفات.
    - **تشفير كلمات المرور:** يتم تشفير كلمات مرور الأعضاء باستخدام تشفير قيصري بسيط.
    - **التحقق من قوة كلمة المرور:** قواعد مختلفة لضمان قوة كلمات المرور للمسؤولين والأعضاء.
    - **إخفاء كلمة المرور:** إخفاء الأحرف عند إدخال كلمة المرور.
    - **جلسات آمنة:** يتضمن مهلة للجلسة عند عدم النشاط ويحد من محاولات تسجيل الدخول للحماية من هجمات التخمين.

### 👨‍💼 صلاحيات مسؤول النظام (Admin)
- إضافة وحذف الكتب من المكتبة.
- إضافة وحذف الأعضاء.
- عرض جميع معاملات الإعارة والإرجاع في النظام.
- إعادة تعيين كلمة مرور أي عضو.

### 🧑‍💻 صلاحيات العضو (Member)
- البحث عن الكتب (حسب العنوان، المؤلف، أو الفئة).
- استعارة الكتب المتاحة.
- إرجاع الكتب المستعارة (مع حساب تلقائي للغرامات عند التأخير).
- عرض سجل الاستعارات الخاص به.
- تغيير كلمة المرور الخاصة به عند أول تسجيل دخول.

## 🛠️ التقنيات المستخدمة
- **لغة البرمجة:** C
- **المفاهيم الرئيسية:**
    - هياكل البيانات (Structs)
    - إدارة الذاكرة الديناميكية (`malloc`, `realloc`)
    - التعامل مع الملفات (`fopen`, `fprintf`, `fscanf`)
    - الترجمة المشروطة للمنصات المختلفة (`#ifdef _WIN32`)

## 🚀 كيفية التشغيل

لترجمة وتشغيل المشروع، ستحتاج إلى مترجم C مثل **GCC**.

1.  **حفظ الكود:**
    احفظ الكود المصدري في ملف باسم `library_system.c`.

2.  **الترجمة (Compilation):**
    افتح الطرفية (Terminal) أو موجه الأوامر (Command Prompt) وانتقل إلى المجلد الذي يحتوي على الملف، ثم قم بترجمته باستخدام الأمر التالي:
    ```bash
    gcc library_system.c -o library_system
    ```

3.  **التشغيل (Execution):**
    بعد انتهاء الترجمة بنجاح، سيتم إنشاء ملف تنفيذي. قم بتشغيله بالأمر:
    -   في Linux/macOS:
        ```bash
        ./library_system
        ```
    -   في Windows:
        ```bash
        library_system.exe
        ```
    سيقوم البرنامج بإنشاء الملفات اللازمة (`books.txt`, `members.txt`, `transactions.txt`) تلقائيًا عند تشغيله لأول مرة.

## 📋 كيفية الاستخدام

عند تشغيل البرنامج، ستظهر لك قائمة رئيسية لاختيار نوع المستخدم:

1.  **تسجيل الدخول كمسؤول (Admin):**
    -   **اسم المستخدم:** `admin`
    -   **كلمة المرور الافتراضية:** `Admin@1234`
    -   سيتمكن المسؤول من الوصول إلى قائمة التحكم الكاملة.

2.  **تسجيل الدخول كعضو (Member):**
    -   يتم إنشاء حسابات الأعضاء بواسطة المسؤول أولاً.
    -   عند أول تسجيل دخول للعضو، سيُطلب منه تغيير كلمة المرور الأولية.

3.  **الخروج:**
    -   لإنهاء البرنامج بأمان.

## 📁 هيكل الملفات
- `library_system.c`: الملف المصدري الرئيسي الذي يحتوي على كل منطق البرنامج.
- `books.txt`: يخزن بيانات مجموعة الكتب في المكتبة.
- `members.txt`: يخزن معلومات حسابات الأعضاء، بما في ذلك كلمات المرور المشفرة.
- `transactions.txt`: يسجل جميع أنشطة الإعارة والإرجاع.
