#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_scan_directory_triggered();

    void on_get_SQLfiles_triggered();

    void on_generate_DirTree_triggered();

    void on_clear_DirTree_triggered();

    void on_get_FileInfo_2_triggered();

    void on_check_InfoChange_triggered();

    void on_sim_DirOper_triggered();

    void on_sim_FileOper_triggered();

    void on_get_SpecInfo_triggered();

    void on_use_document_triggered();

private:
    void setcursor();

public:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
