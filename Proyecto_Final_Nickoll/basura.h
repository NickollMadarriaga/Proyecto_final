#ifndef BASURA_H
#define BASURA_H
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>
#include <QGraphicsScene>
#include <QPixmap>
#include <QRandomGenerator>

class Basura : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum TipoMovimiento {
        Vertical,    // Se mueve hacia abajo (nivel 2)
        Horizontal   // Se mueve hacia la izquierda (nivel 3)
    };

    explicit Basura(TipoMovimiento tipo, QObject *parent = nullptr);
    ~Basura();

    void iniciarMovimiento();
    void detenerMovimiento();
    void configurarComoBasura4();
    TipoMovimiento getTipoMovimiento() const { return tipoMovimiento; }

signals:
    void basuraEliminada(bool tocadaPorGoku); // true si tocó a Goku, false si salió de límites
    void nuevaBasura4Creada(Basura* basura); // Nueva señal para notificar creación de basura4

private slots:
    void mover();

private:
    void configurarPosicionInicial();
    void configurarImagen();
    bool verificarLimites();

    TipoMovimiento tipoMovimiento;
    QTimer *timerMovimiento;
    QTimer *timerBasura4; // Nuevo timer para crear basura4 cada 3 segundos
    int velocidad;
    static const int VELOCIDAD_DEFECTO = 4;

    float centroX;          // Centro X del círculo
    float anguloActual;     // Ángulo actual en radianes
    float radioCirculo;     // Radio del círculo
    float velocidadAngular; // Velocidad de rotación
    bool esBasura4;
};

#endif // BASURA_H
