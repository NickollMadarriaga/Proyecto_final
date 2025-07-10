#include "basura.h"
#include <QDebug>
#include <cmath>

Basura::Basura(TipoMovimiento tipo, QObject *parent)
    : QObject(parent), tipoMovimiento(tipo), velocidad(VELOCIDAD_DEFECTO),
    centroX(0), anguloActual(0), radioCirculo(30), velocidadAngular(0.2), esBasura4(false)
{
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Basura::mover);

    configurarImagen();
    configurarPosicionInicial();
}

void Basura::configurarComoBasura4()
{
    esBasura4 = true;
    // Configurar imagen específica para basura4, una esfera de dragon
    QPixmap pixmap(":/Imagenes/basura4.png");
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
    }

    // Configurar centro X donde empezará el movimiento circular
    centroX = x();

    // Parámetros del movimiento circular, como tal en esta fisica, se cre tipo una espiral bajando, como cuando uno pone una tuerca en un tornillo
    radioCirculo = 25.0f;      // Radio del círculo
    velocidadAngular = 0.30f;  // Velocidad de rotación
    anguloActual = 0.0f;       // Empezar desde 0 grados
}

Basura::~Basura()
{
    if (timerMovimiento) {
        timerMovimiento->stop();
        timerMovimiento->deleteLater();
    }
}

void Basura::configurarImagen()
{
    QStringList imagenes;
    if (tipoMovimiento == Vertical) {
        // Nivel 2: solo basura1.png y basura2.png
        imagenes << ":/Imagenes/basura1.png" << ":/Imagenes/basura2.png";
    } else {
        // Nivel 3: basura1.png, basura2.png y basura3.png
        imagenes << ":/Imagenes/basura1.png" << ":/Imagenes/basura2.png" << ":/Imagenes/basura3.png";
    }

    // Seleccionar imagen aleatoria
    int indice = QRandomGenerator::global()->bounded(imagenes.size());
    QString imagenSeleccionada = imagenes[indice];

    QPixmap pixmap(imagenSeleccionada);
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
    } else {
        qDebug() << "Error: No se pudo cargar la imagen" << imagenSeleccionada;
    }
}

void Basura::configurarPosicionInicial()
{
    if (tipoMovimiento == Vertical) {
        // Nivel 2: y fijo en -50, x entre 10 y 1140
        int xAleatorio = QRandomGenerator::global()->bounded(10, 1141); // 10 a 1140 inclusive
        setPos(xAleatorio, -50);
    } else {
        // Nivel 3: x fijo en 1200, y entre 10 y 665
        int yAleatorio = QRandomGenerator::global()->bounded(10, 666); // 10 a 665 inclusive
        setPos(1200, yAleatorio);
    }
}

void Basura::iniciarMovimiento()
{
    if (timerMovimiento) {
        timerMovimiento->start(50); // Actualizar cada 50ms para movimiento suave
    }
}

void Basura::detenerMovimiento()
{
    if (timerMovimiento) {
        timerMovimiento->stop();
    }
}

void Basura::mover()
{
    if (esBasura4 && tipoMovimiento == Vertical) {
        // Movimiento circular para basura4

        // Calcular nueva posición en el círculo
        float nuevaX = centroX + radioCirculo * cos(anguloActual);
        float nuevaY = y() + velocidad; // Seguir cayendo hacia abajo

        // Actualizar posición
        setPos(nuevaX, nuevaY);

        // Incrementar ángulo para el próximo frame
        anguloActual += velocidadAngular;

        // Mantener el ángulo en el rango [0, 2π]
        if (anguloActual >= 2 * M_PI) {
            anguloActual -= 2 * M_PI;
        }

    } else {
        // Movimiento normal para otras basuras
        if (tipoMovimiento == Vertical) {
            // Nivel 2: mover hacia abajo (sumar velocidad en y)
            setPos(x(), y() + velocidad);
        } else {
            // Nivel 3: mover hacia la izquierda (restar velocidad en x)
            setPos(x() - velocidad, y());
        }
    }

    // Verificar si debe eliminarse por límites
    if (verificarLimites()) {
        emit basuraEliminada(false); // No tocó a Goku, salió de límites
    }
}

bool Basura::verificarLimites()
{
    if (tipoMovimiento == Vertical) {
        // Nivel 2: eliminar si y > 680
        return y() > 680;
    } else {
        // Nivel 3: eliminar si x < -55
        return x() < -55;
    }
}
