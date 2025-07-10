#ifndef INDICADORVIDA_H
#define INDICADORVIDA_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QObject>

class IndicadorVida : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit IndicadorVida(QGraphicsItem *parent = nullptr);

    void setVida(int vida);
    int getVida() const;

    void decrementarVida();
    void incrementarVida();
    void resetearVida();

signals:
    void vidaCambiada(int nuevaVida);
    void vidaAgotada();

private:
    int vidaActual;
    void actualizarImagen();

    QString imagenVida3;
    QString imagenVida2;
    QString imagenVida1;
    QString imagenVida0;
};

#endif // INDICADORVIDA_H
