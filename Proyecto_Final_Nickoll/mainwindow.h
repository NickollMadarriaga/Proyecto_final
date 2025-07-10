#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "personaje.h"
#include <QMainWindow>
#include "indicadorvida.h"
#include "saibaman.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaPlaylist>
#include <QPushButton>
#include <QTimer>
#include <QSet>
#include <QGraphicsPathItem>
#include <QPointer>
#include <QGraphicsTextItem>
#include <QFontDatabase>
#include <QFont>

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
    void actualizarCronometro();

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

    // Sistema de puntuación
    int puntuacion = 0;          // Puntuación actual
    int puntuacionTotal = 0;    // Puntuación guardada de niveles

    // Sistema de cronómetro
    QTimer *timerCronometro = nullptr;
    int tiempoRestante = 60;     // Tiempo en segundos

    // Función para resetear el estado del juego
    void resetearEstadoJuego();
    bool juegoReiniciado = false;

    // SISTEMA DE DESBLOQUEO DE NIVELES
    int nivelMaximoDesbloqueado = 1;  // Cambiar a 2 o 3 para pruebas, volver a 1 para producción

    // Función para crear botones con sistema de bloqueo
    void addButtonWithLock(const QString &imgNormal, const QString &imgHover,
                           const QString &imgBlocked, const QPoint &pos,
                           int nivelRequerido, std::function<void()> cb);

    // Fuente personalizada
    QFont fuentePersonalizada;
    void cargarFuentePersonalizada();

    // Elementos visuales del cronómetro
    QGraphicsPixmapItem *imagenRadar = nullptr;
    QGraphicsTextItem *textoCronometro = nullptr;

    // Elementos visuales del puntaje
    QGraphicsPixmapItem *imagenPuntaje = nullptr;
    QGraphicsTextItem *textoPuntaje = nullptr;

    // Funciones para actualizar UI
    void actualizarTextoCronometro();
    void actualizarTextoPuntaje();
    void crearElementosUI();
    void limpiarElementosUI();
};

#endif // MAINWINDOW_H
