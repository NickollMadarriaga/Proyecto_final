#include "saibaman.h"
#include <QRandomGenerator>
#include <QDebug>

Saibaman::Saibaman(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent),
    anchoFotograma(40),
    altoFotograma(30),
    columnasTotales(4),
    contador(0),
    enAnimacionMuerte(false)
{
    // Cargar el sprite de aparición
    imgSprites.load(":/Imagenes/AparicionSaibaman.png");

    // Cargar el sprite de muerte
    imgMuerte.load(":/Imagenes/MuerteSaibaman.png");

    // Establecer el primer frame
    setPixmap(imgSprites.copy(0, 0, anchoFotograma, altoFotograma));

    // Configurar el timer para la animación de aparición
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Saibaman::actualizarAnimacion);

    // Configurar el timer para la animación de muerte
    timerMuerte = new QTimer(this);
    connect(timerMuerte, &QTimer::timeout, this, &Saibaman::actualizarAnimacionMuerte);

    // Posicionar aleatoriamente y comenzar animación
    posicionarAleatoriamente();
    iniciarAnimacion();
}

Saibaman::~Saibaman()
{
    if (timerAnimacion) {
        timerAnimacion->stop();
    }
    if (timerMuerte) {
        timerMuerte->stop();
    }
}

void Saibaman::posicionarAleatoriamente()
{
    int x = QRandomGenerator::global()->bounded(450, 901);
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
    if (enAnimacionMuerte) return;  // No actualizar si está en animación de muerte

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

void Saibaman::iniciarAnimacionMuerte()
{
    if (enAnimacionMuerte) return;  // Ya está en animación de muerte

    enAnimacionMuerte = true;

    // Detener animación de aparición si está corriendo
    if (timerAnimacion) {
        timerAnimacion->stop();
    }

    // Iniciar animación de muerte
    contadorMuerte = 0;
    timerMuerte->start(150);
    QPointF posicionActual = pos();
    setPos(posicionActual.x() - 25, posicionActual.y() - 25);
}

void Saibaman::actualizarAnimacionMuerte()
{
    // Ajustar posición para compensar el tamaño diferente del sprite de muerte

    // Calcular posición del sprite actual de muerte
    int x = contadorMuerte * anchoMuerte;

    // Asegurarse de que no se salga de los límites
    if (x + anchoMuerte > imgMuerte.width()) {
        x = 0;
    }

    // Actualizar el pixmap con el sprite de muerte
    QPixmap spriteMuerte = imgMuerte.copy(x, 0, anchoMuerte, altoMuerte);
    setPixmap(spriteMuerte);

    contadorMuerte++;

    // Si llegamos al último frame de muerte, parar la animación
    if (contadorMuerte >= columnasMuerte) {
        timerMuerte->stop();
        // Quedarse en el último frame de muerte
        QPixmap ultimoFrameMuerte = imgMuerte.copy((columnasMuerte - 1) * anchoMuerte, 0, anchoMuerte, altoMuerte);
        setPixmap(ultimoFrameMuerte);
        emit animacionMuerteCompleta();
    }
}
