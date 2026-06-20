#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <chrono>
#include <deque>
#include <sstream>
#include <iomanip>

class Stopwatch : public QMainWindow {
    Q_OBJECT
public:
    Stopwatch() {
        auto *central = new QWidget(this);
        central->setStyleSheet("background-color: #1e1e2e;"); // dark modern background
        auto *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(15);

        // Time display
        timeLabel = new QLabel("0.00 s");
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setStyleSheet(
            "font-size: 48px; font-weight: bold; color: #cdd6f4; "
            "padding: 20px; background-color: #313244; border-radius: 12px;"
        );
        mainLayout->addWidget(timeLabel);

        // Button row
        auto *btnLayout = new QHBoxLayout();
        btnLayout->setSpacing(10);
        startBtn = createButton("▶ Start", "#a6e3a1");
        stopBtn  = createButton("⏹ Stop", "#f38ba8");
        lapBtn   = createButton("⏱ Lap", "#89b4fa");
        resetBtn = createButton("↺ Reset", "#f9e2af");

        stopBtn->setEnabled(false);
        lapBtn->setEnabled(false);

        btnLayout->addWidget(startBtn);
        btnLayout->addWidget(stopBtn);
        btnLayout->addWidget(lapBtn);
        btnLayout->addWidget(resetBtn);
        mainLayout->addLayout(btnLayout);

        // Recorded times list
        timesList = new QListWidget();
        timesList->setStyleSheet(
            "QListWidget { background-color: #313244; border-radius: 8px; "
            "color: #cdd6f4; font-size: 16px; padding: 10px; }"
            "QListWidget::item { padding: 6px; border-bottom: 1px solid #45475a; }"
        );
        mainLayout->addWidget(timesList);

        setCentralWidget(central);
        setWindowTitle("Stopwatch");
        resize(400, 500);
        setStyleSheet("QMainWindow { background-color: #1e1e2e; }");

        // Timer
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Stopwatch::updateDisplay);

        // Button connections
        connect(startBtn, &QPushButton::clicked, this, &Stopwatch::start);
        connect(stopBtn, &QPushButton::clicked, this, &Stopwatch::stop);
        connect(lapBtn, &QPushButton::clicked, this, &Stopwatch::lap);
        connect(resetBtn, &QPushButton::clicked, this, &Stopwatch::reset);
    }

private slots:
    void start() {
        if (!running) {
            running = true;
            startTime = std::chrono::steady_clock::now();
            timer->start(100);
            startBtn->setEnabled(false);
            stopBtn->setEnabled(true);
            lapBtn->setEnabled(true);
        }
    }

    void stop() {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            accumulated += now - startTime;
            running = false;
            timer->stop();
            double secs = std::chrono::duration<double>(accumulated).count();
            addTime(secs);

            startBtn->setEnabled(true);
            stopBtn->setEnabled(false);
            lapBtn->setEnabled(false);
        }
    }

    void lap() {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = accumulated + (now - startTime);
            double secs = std::chrono::duration<double>(elapsed).count();
            addTime(secs);
        }
    }

    void reset() {
        timer->stop();
        running = false;
        accumulated = std::chrono::duration<double>(0);
        lapTimes.clear();
        timeLabel->setText("0.00 s");
        timesList->clear();

        startBtn->setEnabled(true);
        stopBtn->setEnabled(false);
        lapBtn->setEnabled(false);
    }

    void updateDisplay() {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = accumulated + (now - startTime);
            double secs = std::chrono::duration<double>(elapsed).count();
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << secs << " s";
            timeLabel->setText(QString::fromStdString(ss.str()));
        }
    }

private:
    void addTime(double secs) {
        lapTimes.push_back(secs);
        if (lapTimes.size() > 10)
            lapTimes.pop_front();

        timesList->clear();
        for (double t : lapTimes) {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << t << " s";
            timesList->addItem(QString::fromStdString(ss.str()));
        }
    }

    QPushButton* createButton(const QString& text, const QString& color) {
        auto *btn = new QPushButton(text);
        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: #1e1e2e; "
                    "font-size: 16px; font-weight: bold; padding: 12px 20px; "
                    "border-radius: 8px; border: none; }"
                    "QPushButton:hover { background-color: %1; opacity: 0.8; }"
                    "QPushButton:pressed { background-color: %1; opacity: 0.6; }"
                    "QPushButton:disabled { background-color: #585b70; color: #6c7086; }")
            .arg(color));
        return btn;
    }

    QLabel *timeLabel;
    QPushButton *startBtn, *stopBtn, *lapBtn, *resetBtn;
    QListWidget *timesList;
    QTimer *timer;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::duration<double> accumulated{0};
    bool running = false;
    std::deque<double> lapTimes;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Stopwatch w;
    w.show();
    return app.exec();
}

#include "Stopwatch.moc"