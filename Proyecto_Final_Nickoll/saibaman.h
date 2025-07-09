#ifndef SAIBAMAN_H
#define SAIBAMAN_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QPixmap>

class Saibaman : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Saibaman(QGraphicsItem *parent = nullptr);
    ~Saibaman();

    void posicionarAleatoriamente();

private slots:
    void actualizarAnimacion();

private:
    QPixmap imgSprites;
    QTimer *timerAnimacion;

    int anchoFotograma;
    int altoFotograma;
    int columnasTotales;
    int contador;

    void iniciarAnimacion();

signals:
    void animacionCompleta();
};

#endif // SAIBAMAN_H
