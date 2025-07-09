#include "saibaman.h"
#include <QRandomGenerator>
#include <QDebug>

Saibaman::Saibaman(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent),
    anchoFotograma(40),
    altoFotograma(30),
    columnasTotales(4),
    contador(0)
{
    // Cargar el sprite
    imgSprites.load(":/Imagenes/AparicionSaibaman.png");

    // Establecer el primer frame
    setPixmap(imgSprites.copy(0, 0, anchoFotograma, altoFotograma));

    // Configurar el timer para la animación
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Saibaman::actualizarAnimacion);

    // Posicionar aleatoriamente y comenzar animación
    posicionarAleatoriamente();
    iniciarAnimacion();
}

Saibaman::~Saibaman()
{
    if (timerAnimacion) {
        timerAnimacion->stop();
    }
}

void Saibaman::posicionarAleatoriamente()
{
    int x = QRandomGenerator::global()->bounded(450, 1101);
    int y = 400;

    setPos(x, y);
}

void Saibaman::iniciarAnimacion()
{
    contador = 0;
    timerAnimacion->start(150); // Velocidad de animación similar a Personaje
}

void Saibaman::actualizarAnimacion()
{
    // Calcular posición del sprite actual
    int x = contador * anchoFotograma;

    // Asegurarse de que no se salga de los límites
    if (x + anchoFotograma > imgSprites.width()) {
        x = 0;
    }

    // Actualizar el pixmap
    QPixmap spriteActual = imgSprites.copy(x, 0, anchoFotograma, altoFotograma);
    setPixmap(spriteActual);

    contador++;

    // Si llegamos al último frame (frame 4), parar la animación
    if (contador >= columnasTotales) {
        timerAnimacion->stop();
        // Quedarse en el último frame
        QPixmap ultimoFrame = imgSprites.copy((columnasTotales - 1) * anchoFotograma, 0, anchoFotograma, altoFotograma);
        setPixmap(ultimoFrame);
        emit animacionCompleta();
    }
}
