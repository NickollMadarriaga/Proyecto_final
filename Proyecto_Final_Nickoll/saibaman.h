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
    void iniciarAnimacionMuerte();

private slots:
    void actualizarAnimacion();
    void actualizarAnimacionMuerte();

private:
    QPixmap imgSprites;
    QPixmap imgMuerte;
    QTimer *timerAnimacion;
    QTimer *timerMuerte;
    int anchoFotograma;
    int altoFotograma;
    int columnasTotales;
    int contador;

    int anchoMuerte = 79;
    int altoMuerte = 84;
    int columnasMuerte = 4;
    int contadorMuerte = 0;
    bool enAnimacionMuerte = false;

    void iniciarAnimacion();

signals:
    void animacionCompleta();
    void animacionMuerteCompleta();
};

#endif // SAIBAMAN_H
