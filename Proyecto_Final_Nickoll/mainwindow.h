#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "personaje.h"
#include "indicadorvida.h"
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
    void actualizarParabola();
    void manejarFinAnimacionEvento();

private:
    Ui::MainWindow *ui;

    QGraphicsView *vista = nullptr;
    QGraphicsScene *scene = nullptr;

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
    QSet<int> teclasPresionadas;

    bool controlesActivos = false;
    bool permitirMovimientoVertical = true;

    // Pal nivel 1
    int contadorEspacio = 0;
    QGraphicsPathItem *lineaParabola = nullptr;
    QTimer *timerParabola = nullptr;
    double angulo = 10.0;
    double velocidad = 70.0;
    bool anguloFijado = false;
    bool velocidadFijada = false;
    bool subiendoAngulo = true;
    bool subiendoVelocidad = true;
    bool aumentandoAngulo = true;
    bool aumentandoVelocidad = true;
    double anguloBala = 0.0;
    double velocidadBala = 0.0;

    IndicadorVida *indicadorVida;
    int vida;

};

#endif // MAINWINDOW_H
