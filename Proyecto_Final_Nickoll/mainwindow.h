#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "personaje.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QList>
#include <functional>

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

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;


private slots:
    void Inicio();
    void Nivel1();
    void Nivel2();
    void Nivel3();
    void limpiarMenu();

private:
    Ui::MainWindow *ui;

    QGraphicsView *vista = nullptr;

    // Multimedia del menú
    QMediaPlayer *videoPlayer = nullptr;
    QMediaPlayer *audioPlayer = nullptr;
    QVideoWidget *videoWidget = nullptr;

    // Botones del menú
    QList<QPushButton*> menuButtons;

    // Botón genérico con efecto de brillo
    void addTransparentButton(const QString &imagenNormal,
                              const QString &imagenHover,
                              const QPoint &posicion,
                              std::function<void()> onClick);

    // Botón para volver desde un nivel al menú
    void agregarBotonVolver();
    // De la clase propia 'personaje' creamos al maravilloso goku
    Personaje *goku;

    bool teclaA_presionada = false;
    bool teclaD_presionada = false;
    bool controlesActivos = false;

};

#endif // MAINWINDOW_H
