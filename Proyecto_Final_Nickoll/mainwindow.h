#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "personaje.h"
#include <QPointer>
#include "indicadorvida.h"
#include "saibaman.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPushButton>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QList>
#include <QSet>
#include <QTimer>
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
    void crearNuevoSaibaman();

private:
    Ui::MainWindow *ui;

    // Vista y escena del juego
    QGraphicsView *vista = nullptr;
    QGraphicsScene *scene = nullptr;

    // Multimedia del menú
    QMediaPlayer *videoPlayer = nullptr;
    QMediaPlayer *audioPlayer = nullptr;
    QVideoWidget *videoWidget = nullptr;

    // Botones del menú
    QList<QPushButton*> menuButtons;

    // Funciones para botones
    void addTransparentButton(const QString &imagenNormal,
                              const QString &imagenHover,
                              const QPoint &posicion,
                              std::function<void()> onClick);
    void agregarBotonVolver();

    // Función para limpiar todo
    void limpiarTodo();

    // Personajes del juego
    Personaje *goku = nullptr;
    Saibaman *saibaman = nullptr;

    // Control de teclas
    QSet<int> teclasPresionadas;
    bool controlesActivos = false;
    bool permitirMovimientoVertical = true;

    // Variables para el nivel 1 (sistema de parábola)
    int contadorEspacio = 0;
    QGraphicsPathItem *lineaParabola = nullptr;
    QTimer *timerParabola = nullptr;
    double angulo = 10.0;
    double velocidad = 70.0;
    bool anguloFijado = false;
    bool velocidadFijada = false;
    bool aumentandoAngulo = true;
    bool aumentandoVelocidad = true;
    double anguloBala = 0.0;
    double velocidadBala = 0.0;

    // Sistema de vida
    IndicadorVida *indicadorVida = nullptr;
    int vida = 3;

    // Función para resetear el estado del juego
    void resetearEstadoJuego();
    bool juegoReiniciado = false;

};

#endif // MAINWINDOW_H
