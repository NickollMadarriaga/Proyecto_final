#ifndef SAIBAMAN_H
#define SAIBAMAN_H
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QRandomGenerator>

class Saibaman : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Saibaman(QGraphicsItem *parent = nullptr);
    ~Saibaman();
    void posicionarAleatoriamente();
    void iniciarAnimacionMuerte();  // Función para animación de muerte

private slots:
    void actualizarAnimacion();
    void actualizarAnimacionMuerte();  // Slot para animación de muerte

private:
    QPixmap imgSprites;
    QPixmap imgMuerte;  // Sprite de muerte
    QTimer *timerAnimacion;
    QTimer *timerMuerte;  // Timer para animación de muerte
    int anchoFotograma;
    int altoFotograma;
    int columnasTotales;
    int contador;

    // Variables para animación de muerte
    int anchoMuerte = 79;
    int altoMuerte = 84;
    int columnasMuerte = 4;
    int contadorMuerte = 0;
    bool enAnimacionMuerte = false;

    void iniciarAnimacion();

signals:
    void animacionCompleta();
    void animacionMuerteCompleta();  // Nueva señal para cuando termine la animación de muerte
};

#endif // SAIBAMAN_H
