#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <map>
#include <numeric>

struct Lesson {
    std::string teacher;    // ФИО преподавателя
    int week;               // Номер недели
    int day;                // День проведения пары (1–7)
    int start_hour;         // Время начала пары (в часах)
    int duration;           // Длительность пары (в часах)
};

// Глобальный mutex для защиты вывода
std::mutex cout_mutex;

// Функция для подсчёта рабочих часов преподавателя
int calculateHours(const std::vector<Lesson>& schedule, const std::string& teacher, int week) {
    int total_hours = 0;
    for (const auto& lesson : schedule) {
        if (lesson.teacher == teacher && lesson.week == week) {
            total_hours += lesson.duration;
        }
    }
    return total_hours;
}

// Обёртка для многопоточной обработки
void threadTask(const std::vector<Lesson>& schedule, const std::string& teacher, int week, int& result) {
    result = calculateHours(schedule, teacher, week);
}

int main() {
    // Пример данных
    std::vector<Lesson> schedule = {
        {"Иванов И.И.", 1, 1, 9, 2},
        {"Иванов И.И.", 1, 3, 11, 3},
        {"Петров П.П.", 1, 2, 10, 1},
        {"Иванов И.И.", 1, 5, 14, 2},
        {"Петров П.П.", 2, 1, 9, 2},
    };

    std::string teacher = "Иванов И.И.";
    int week = 1;

    // Переменные для времени обработки
    auto start = std::chrono::high_resolution_clock::now();
    int single_thread_result = calculateHours(schedule, teacher, week);
    auto end = std::chrono::high_resolution_clock::now();
    auto single_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Многопоточная обработка
    start = std::chrono::high_resolution_clock::now();
    int multi_thread_result = 0;
    std::thread t(threadTask, std::cref(schedule), teacher, week, std::ref(multi_thread_result));
    t.join();
    end = std::chrono::high_resolution_clock::now();
    auto multi_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Вывод результатов
    std::cout << "Результат (один поток): " << single_thread_result << " часов, время: " 
              << single_thread_time << " мкс\n";
    std::cout << "Результат (многопоточность): " << multi_thread_result << " часов, время: " 
              << multi_thread_time << " мкс\n";

    return 0;
}
