

## تحليل الدوال

### `void caesar_encrypt(const char *input, char *output)`

#### الشرح بالعربية
تقوم بتشفير سلسلة نصية معينة باستخدام تشفير قيصر بسيط بقيمة إزاحة محددة مسبقًا (CAESAR_SHIFT). تقوم بإزاحة كل حرف بإضافة قيمة الإزاحة.

#### Explanation in English
Encrypts a given string using a simple Caesar cipher with a predefined shift value (CAESAR_SHIFT). It shifts each character by adding the shift value.

---

### `void caesar_decrypt(const char *input, char *output)`

#### الشرح بالعربية
تقوم بفك تشفير سلسلة نصية معينة تم تشفيرها باستخدام تشفير قيصر. تقوم بإزاحة كل حرف للخلف بطرح قيمة الإزاحة المحددة مسبقًا (CAESAR_SHIFT).

#### Explanation in English
Decrypts a given string that was encrypted using the Caesar cipher. It shifts each character back by subtracting the predefined shift value (CAESAR_SHIFT).

---

### `void enable_virtual_terminal_processing()`

#### الشرح بالعربية
تقوم بتمكين معالجة الطرفية الافتراضية لوحدة التحكم على أنظمة Windows، مما يسمح باستخدام أكواد ANSI للألوان في الإخراج. لا تفعل شيئًا على أنظمة التشغيل الأخرى.

#### Explanation in English
Enables virtual terminal processing for the console on Windows systems, allowing ANSI escape codes for colored output. It does nothing on other operating systems.

---

### `void clear_screen()`

#### الشرح بالعربية
تقوم بمسح شاشة وحدة التحكم باستخدام أوامر خاصة بالنظام (cls لنظام Windows، وclear للأنظمة الشبيهة بيونكس).

#### Explanation in English
Clears the console screen using system-specific commands (cls for Windows, clear for Unix-like systems).

---

### `void clear_input_buffer()`

#### الشرح بالعربية
تقوم بمسح أي أحرف متبقية في مخزن الإدخال القياسي المؤقت، وتستخدم عادةً بعد scanf أو getchar لمنع بقايا الأسطر الجديدة من التأثير على استدعاءات fgets اللاحقة.

#### Explanation in English
Clears any remaining characters in the standard input buffer, typically used after scanf or getchar to prevent leftover newlines from affecting subsequent fgets calls.

---

### `void press_enter_to_continue()`

#### الشرح بالعربية
تطلب من المستخدم الضغط على مفتاح Enter للمتابعة وتنتظر ضغط المفتاح.

#### Explanation in English
Prompts the user to press Enter to continue execution and waits for the Enter key press.

---

### `void get_string_input(const char *prompt, char *buffer, int size)`

#### الشرح بالعربية
تطلب من المستخدم إدخال نص برسالة معينة وتقرأ سطرًا من الإدخال النصي في مخزن مؤقت، مع إزالة حرف السطر الجديد الزائد.

#### Explanation in English
Prompts the user with a given message and reads a line of text input into a buffer, removing the trailing newline character.

---

### `int get_int_input(const char *prompt)`

#### الشرح بالعربية
تطلب من المستخدم إدخال عدد صحيح وتستمر في الطلب حتى يتم إدخال عدد صحيح صالح.

#### Explanation in English
Prompts the user for an integer input and repeatedly asks until a valid integer is entered.

---

### `void get_masked_password(const char *prompt, char *buffer, int size)`

#### الشرح بالعربية
تطلب من المستخدم إدخال كلمة مرور، وتعرض علامات نجمية (*) بدلاً من الأحرف الفعلية أثناء الكتابة (إخفاء). تتعامل مع مفتاحي المسافة الخلفية والسطر الجديد.

#### Explanation in English
Prompts the user for a password input, displaying asterisks (*) instead of the actual characters as they are typed (masking). It handles backspace and newline characters.

---

### `void lock_file(FILE *fp)`

#### الشرح بالعربية
تحصل على قفل كتابة حصري على مؤشر الملف المحدد، مما يمنع العمليات الأخرى من الكتابة إليه في نفس الوقت. تستخدم آليات قفل خاصة بالمنصة.

#### Explanation in English
Acquires an exclusive write lock on the specified file pointer, preventing other processes from writing to it simultaneously. It uses platform-specific locking mechanisms.

---

### `void unlock_file(FILE *fp)`

#### الشرح بالعربية
تحرر قفل الكتابة الحصري على مؤشر الملف المحدد، مما يسمح للعمليات الأخرى بالوصول إليه. تستخدم آليات تحرير قفل خاصة بالمنصة.

#### Explanation in English
Releases the exclusive write lock on the specified file pointer, allowing other processes to access it. It uses platform-specific unlocking mechanisms.

---

### `void load_books()`

#### الشرح بالعربية
تقوم بتحميل بيانات الكتب من ملف BOOK_FILE إلى مصفوفة books العالمية. تعيد تخصيص الذاكرة ديناميكيًا حسب الحاجة وتحدث next_book_id.

#### Explanation in English
Loads book data from the BOOK_FILE into the global books array. It dynamically reallocates memory as needed and updates the next_book_id.

---

### `void save_books()`

#### الشرح بالعربية
تقوم بحفظ جميع بيانات الكتب من مصفوفة books العالمية إلى ملف BOOK_FILE. تطبق قفلًا على الملف أثناء الكتابة لضمان سلامة البيانات.

#### Explanation in English
Saves all book data from the global books array to the BOOK_FILE. It applies a file lock during writing to ensure data integrity.

---

### `void load_members()`

#### الشرح بالعربية
تقوم بتحميل بيانات الأعضاء من ملف MEMBER_FILE إلى مصفوفة members العالمية. تعيد تخصيص الذاكرة ديناميكيًا حسب الحاجة وتحدث next_member_id.

#### Explanation in English
Loads member data from the MEMBER_FILE into the global members array. It dynamically reallocates memory as needed and updates the next_member_id.

---

### `void save_members()`

#### الشرح بالعربية
تقوم بحفظ جميع بيانات الأعضاء من مصفوفة members العالمية إلى ملف MEMBER_FILE. تطبق قفلًا على الملف أثناء الكتابة لضمان سلامة البيانات.

#### Explanation in English
Saves all member data from the global members array to the MEMBER_FILE. It applies a file lock during writing to ensure data integrity.

---

### `void load_transactions()`

#### الشرح بالعربية
تقوم بتحميل بيانات المعاملات من ملف TRANSACTION_FILE إلى مصفوفة transactions العالمية. تتعامل مع تحويل time_t، وتعيد تخصيص الذاكرة ديناميكيًا، وتحدث next_transaction_id.

#### Explanation in English
Loads transaction data from the TRANSACTION_FILE into the global transactions array. It handles time_t conversion, dynamically reallocates memory, and updates next_transaction_id.

---

### `void save_transactions()`

#### الشرح بالعربية
تقوم بحفظ جميع بيانات المعاملات من مصفوفة transactions العالمية إلى ملف TRANSACTION_FILE. تطبق قفلًا على الملف أثناء الكتابة لضمان سلامة البيانات.

#### Explanation in English
Saves all transaction data from the global transactions array to the TRANSACTION_FILE. It applies a file lock during writing to ensure data integrity.

---

### `Book *find_book_by_id(int id)`

#### الشرح بالعربية
تبحث عن كتاب في مصفوفة books العالمية باستخدام معرفه وتعيد مؤشرًا إلى الكتاب إذا تم العثور عليه، وإلا تعيد NULL.

#### Explanation in English
Searches for a book in the global books array by its ID and returns a pointer to the book if found, otherwise returns NULL.

---

### `Member *find_member_by_id(int id)`

#### الشرح بالعربية
تبحث عن عضو في مصفوفة members العالمية باستخدام معرفه وتعيد مؤشرًا إلى العضو إذا تم العثور عليه، وإلا تعيد NULL.

#### Explanation in English
Searches for a member in the global members array by their ID and returns a pointer to the member if found, otherwise returns NULL.

---

### `Member *find_member_by_name(const char *name)`

#### الشرح بالعربية
تبحث عن عضو في مصفوفة members العالمية باستخدام اسمه وتعيد مؤشرًا إلى العضو إذا تم العثور عليه، وإلا تعيد NULL.

#### Explanation in English
Searches for a member in the global members array by their name and returns a pointer to the member if found, otherwise returns NULL.

---

### `int is_strong_admin_password(const char *pass)`

#### الشرح بالعربية
تتحقق مما إذا كانت كلمة المرور المعطاة تستوفي متطلبات كلمة المرور القوية للمسؤول: 12 حرفًا كحد أدنى، وحرف كبير واحد على الأقل، وحرف صغير واحد، ورقم واحد، وحرف خاص واحد.

#### Explanation in English
Checks if a given password meets the strong password requirements for an admin: minimum 12 characters, at least one uppercase letter, one lowercase letter, one digit, and one special character.

---

### `int is_valid_member_password(const char *pass)`

#### الشرح بالعربية
تتحقق مما إذا كانت كلمة المرور المعطاة تستوفي متطلبات الصلاحية للعضو: 8 أحرف كحد أدنى ورقم واحد على الأقل.

#### Explanation in English
Checks if a given password meets the validity requirements for a member: minimum 8 characters and at least one digit.

---

### `void display_books_paginated(int current_page)`

#### الشرح بالعربية
تعرض قائمة كتب مقسمة على صفحات من مصفوفة books العالمية، حيث تعرض ITEMS_PER_PAGE كتابًا لكل صفحة.

#### Explanation in English
Displays a paginated list of books from the global books array, showing ITEMS_PER_PAGE books per page.

---

### `void display_transactions_paginated(int current_page)`

#### الشرح بالعربية
تعرض قائمة معاملات مقسمة على صفحات من مصفوفة transactions العالمية، حيث تعرض ITEMS_PER_PAGE معاملة لكل صفحة. تقوم بتنسيق تواريخ الاستعارة والإرجاع.

#### Explanation in English
Displays a paginated list of transactions from the global transactions array, showing ITEMS_PER_PAGE transactions per page. It formats borrow and return dates.

---

### `void add_book()`

#### الشرح بالعربية
تطلب من المستخدم تفاصيل لإضافة كتاب جديد إلى المكتبة. تقوم بتعيين معرف جديد، وجمع العنوان والمؤلف والفئة والكمية، ثم تحفظ قائمة الكتب المحدثة.

#### Explanation in English
Prompts the user for details to add a new book to the library. It assigns a new ID, collects title, author, category, and quantity, then saves the updated book list.

---

### `void delete_book()`

#### الشرح بالعربية
تطلب من المستخدم معرف كتاب وتحذف الكتاب المقابل من سجلات المكتبة، ثم تحفظ قائمة الكتب المحدثة.

#### Explanation in English
Prompts the user for a book ID and deletes the corresponding book from the library's records, then saves the updated book list.

---

### `void list_all_books()`

#### الشرح بالعربية
تسمح للمستخدم بتصفح جميع الكتب في عرض مقسم على صفحات، والتنقل بين الصفحات باستخدام 'N' (التالي)، 'P' (السابق)، أو 'Q' (الخروج).

#### Explanation in English
Allows the user to browse all books in a paginated view, navigating through pages using 'N' (next), 'P' (previous), or 'Q' (quit).

---

### `void add_member()`

#### الشرح بالعربية
تطلب من المستخدم تفاصيل لإضافة عضو جديد إلى المكتبة. تقوم بتعيين معرف جديد، وجمع الاسم والبريد الإلكتروني وكلمة مرور أولية (التي يتم تشفيرها ووضع علامة عليها لتغيير كلمة المرور عند أول تسجيل دخول)، ثم تحفظ قائمة الأعضاء المحدثة.

#### Explanation in English
Prompts the user for details to add a new member to the library. It assigns a new ID, collects name, email, and an initial password (which is encrypted and marked for first login password change), then saves the updated member list.

---

### `void delete_member()`

#### الشرح بالعربية
تطلب من المستخدم معرف عضو وتحذف العضو المقابل من سجلات المكتبة، ثم تحفظ قائمة الأعضاء المحدثة.

#### Explanation in English
Prompts the user for a member ID and deletes the corresponding member from the library's records, then saves the updated member list.

---

### `void view_all_transactions()`

#### الشرح بالعربية
تسمح للمسؤول بتصفح جميع معاملات المكتبة في عرض مقسم على صفحات، والتنقل بين الصفحات.

#### Explanation in English
Allows the admin to browse all library transactions in a paginated view, navigating through pages.

---

### `void reset_member_password()`

#### الشرح بالعربية
تسمح للمسؤول بإعادة تعيين كلمة مرور عضو. تطلب معرف العضو، وتتحقق من صحة كلمة المرور الجديدة، وتقوم بتشفيرها، وتضع علامة على العضو لتغيير كلمة المرور إلزاميًا عند تسجيل الدخول التالي.

#### Explanation in English
Allows an admin to reset a member's password. It prompts for the member ID, validates the new password, encrypts it, and marks the member for a mandatory password change on next login.

---

### `void search_books()`

#### الشرح بالعربية
تسمح للمستخدمين بالبحث عن الكتب حسب العنوان أو المؤلف أو الفئة. تعرض الكتب المطابقة، وتقوم ببحث عن جزء من السلسلة غير حساس لحالة الأحرف.

#### Explanation in English
Allows users to search for books by title, author, or category. It displays matching books, performing a case-insensitive substring search.

---

### `void borrow_book(int member_id)`

#### الشرح بالعربية
تسمح للعضو باستعارة كتاب. تعرض الكتب المتاحة مقسمة على صفحات، وتطلب معرف الكتاب، وتنشئ سجل معاملة جديدًا، وتحدث توفر الكتاب، وتحسب تاريخ الاستحقاق.

#### Explanation in English
Allows a member to borrow a book. It displays available books paginated, prompts for a book ID, creates a new transaction record, updates book availability, and calculates the due date.

---

### `void return_book(int member_id)`

#### الشرح بالعربية
تسمح للعضو بإرجاع كتاب مستعار. تسرد الكتب المستعارة حاليًا للعضو، وتطلب معرف المعاملة، وتحدث سجل المعاملة بتاريخ الإرجاع، وتحسب أي غرامات تأخير، وتزيد الكمية المتاحة من الكتاب.

#### Explanation in English
Allows a member to return a borrowed book. It lists the member's currently borrowed books, prompts for a transaction ID, updates the transaction record with the return date, calculates any overdue fines, and increases the book's available quantity.

---

### `void view_my_records(int member_id)`

#### الشرح بالعربية
تعرض جميع سجلات المعاملات (الكتب المستعارة والمرجعة، بما في ذلك الغرامات) لعضو معين.

#### Explanation in English
Displays all transaction records (borrowed and returned books, including fines) for a specific member.

---

### `int check_session_timeout()`

#### الشرح بالعربية
تتحقق مما إذا انتهت صلاحية جلسة المستخدم بسبب عدم النشاط. إذا انتهت الصلاحية، تقوم بتسجيل خروج المستخدم وتعيد 1؛ وإلا، تقوم بتحديث وقت آخر نشاط وتعيد 0.

#### Explanation in English
Checks if the user's session has timed out due to inactivity. If timed out, it logs out the user and returns 1; otherwise, it updates the last activity time and returns 0.

---

### `void change_password(Member *member, int is_admin)`

#### الشرح بالعربية
تتعامل مع عملية تغيير كلمة مرور المستخدم، وتفرض متطلبات قوة مختلفة للمسؤولين والأعضاء العاديين. تطلب كلمة مرور جديدة، وتؤكدها، وتقوم بتشفيرها، وتحدث حالة is_first_login للعضو.

#### Explanation in English
Handles the process of changing a user's password, enforcing different strength requirements for admin and regular members. It prompts for a new password, confirms it, encrypts it, and updates the member's is_first_login status.

---

### `void login()`

#### الشرح بالعربية
تدير عملية تسجيل دخول المستخدم. تطلب نوع المستخدم (أمين مكتبة/عضو)، واسم المستخدم، وكلمة المرور. تتحقق من صحة البيانات، وتتعامل مع المحاولات الخاطئة، وتعيد التوجيه إلى القائمة المناسبة أو تفرض تغيير كلمة المرور إذا كان هذا هو أول تسجيل دخول.

#### Explanation in English
Manages the user login process. It prompts for user type (librarian/member), username, and password. It validates credentials, handles incorrect attempts, and redirects to the appropriate menu or forces a password change if it's the first login.

---

### `void admin_menu()`

#### الشرح بالعربية
تعرض القائمة الرئيسية لأمين المكتبة (المسؤول) وتتعامل مع خياراته، بما في ذلك إدارة الكتب والأعضاء والمعاملات. كما تتحقق من انتهاء صلاحية الجلسة.

#### Explanation in English
Displays the main menu for the librarian (admin) and handles their choices, including managing books, members, and transactions. It also checks for session timeouts.

---

### `void member_menu(int member_id)`

#### الشرح بالعربية
تعرض القائمة الرئيسية للعضو المسجل دخوله وتتعامل مع خياراته، بما في ذلك البحث عن الكتب واستعارتها وإرجاعها وعرض سجلاته. كما تتحقق من انتهاء صلاحية الجلسة.

#### Explanation in English
Displays the main menu for a logged-in member and handles their choices, including searching, borrowing, returning books, and viewing their records. It also checks for session timeouts.

---

### `void initialize_system()`

#### الشرح بالعربية
تهيئ نظام إدارة المكتبة عن طريق تحميل البيانات من ملفات الكتب والأعضاء والمعاملات. إذا لم يتم العثور على أعضاء، فإنها تنشئ حساب مسؤول افتراضي.

#### Explanation in English
Initializes the library system by loading data from book, member, and transaction files. If no members are found, it creates a default admin account.

---

### `int main()`

#### الشرح بالعربية
نقطة الدخول الرئيسية للبرنامج. تقوم بتهيئة النظام، وتمكين معالجة الطرفية الافتراضية (للألوان)، وتقديم قائمة تسجيل الدخول/الخروج الرئيسية، وتدير دورة حياة البرنامج بما في ذلك تحرير الذاكرة المخصصة قبل الخروج.

#### Explanation in English
The entry point of the program. It initializes the system, enables virtual terminal processing (for colors), presents the main login/exit menu, and manages the program's lifecycle including freeing allocated memory before exiting.

---

