#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bala.h"
#include "indicadorvida.h"
#include <QKeyEvent>
#include <QMediaPlaylist>
#include <QPixmap>
#include <QBitmap>
#include <QApplication>
#include <QDebug>
#include <QtMath>

class HoverIconEventFilter : public QObject
{
    QPixmap normal, hover;
    QPointer<QPushButton> boton;  // Usar QPointer para detectar eliminación
public:
    HoverIconEventFilter(QPixmap normalImg, QPixmap hoverImg, QPushButton *btn)
        : QObject(btn), normal(normalImg), hover(hoverImg), boton(btn) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (!boton) return false;  // Verificar que el botón exista

        if (event->type() == QEvent::Enter) {
            boton->setIcon(QIcon(hover));
        } else if (event->type() == QEvent::Leave) {
            boton->setIcon(QIcon(normal));
        }
        return false;
    }
};

class HoverIconEventFilterWithLock : public QObject
{
    QPixmap normal, hover, blocked;
    QPointer<QPushButton> boton;
    bool isBlocked;

public:
    HoverIconEventFilterWithLock(QPixmap normalImg, QPixmap hoverImg, QPixmap blockedImg, QPushButton *btn, bool blocked)
        : QObject(btn), normal(normalImg), hover(hoverImg), blocked(blockedImg), boton(btn), isBlocked(blocked) {}

    void setBlocked(bool isButtonBlocked) {
        isBlocked = isButtonBlocked;
        if (boton) {
            if (isBlocked) {
                boton->setIcon(QIcon(blocked));
            } else {
                boton->setIcon(QIcon(normal));
            }
        }
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (!boton || isBlocked) return false;

        if (event->type() == QEvent::Enter) {
            boton->setIcon(QIcon(hover));
        } else if (event->type() == QEvent::Leave) {
            boton->setIcon(QIcon(normal));
        }
        return false;
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), vida(3), puntuacion(0), puntuacionTotal(0), tiempoRestante(60)
{
    ui->setupUi(this);
    this->resize(1200, 675);

    // Cargar fuente personalizada
    cargarFuentePersonalizada();

    // Inicializar punteros a nullptr
    videoPlayer = nullptr;
    audioPlayer = nullptr;
    videoWidget = nullptr;
    vista = nullptr;
    scene = nullptr;
    goku = nullptr;
    saibaman = nullptr;
    lineaParabola = nullptr;
    timerParabola = nullptr;
    indicadorVida = nullptr;
    timerCronometro = nullptr;

    // Inicializar nuevos elementos UI
    imagenRadar = nullptr;
    textoCronometro = nullptr;
    imagenPuntaje = nullptr;
    textoPuntaje = nullptr;

    Inicio();
}

void MainWindow::cargarFuentePersonalizada()
{
    // Cargar la fuente desde recursos
    int fontId = QFontDatabase::addApplicationFont(":/Fuentes/Saiyan-Sans.ttf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.empty()) {
            fuentePersonalizada = QFont(fontFamilies.at(0), 24, QFont::Bold);
            qDebug() << "Fuente personalizada cargada:" << fontFamilies.at(0);
        } else {
            qDebug() << "Error: No se pudo obtener la familia de fuentes";
            fuentePersonalizada = QFont("Arial", 24, QFont::Bold); // Fuente por defecto
        }
    } else {
        qDebug() << "Error: No se pudo cargar la fuente desde recursos";
        fuentePersonalizada = QFont("Arial", 24, QFont::Bold); // Fuente por defecto
    }
}

void MainWindow::crearElementosUI()
{
    if (!scene) return;

    // Crear imagen del radar (cronómetro)
    imagenRadar = new QGraphicsPixmapItem(QPixmap(":/Imagenes/radar.png"));
    imagenRadar->setPos(825, 22);
    imagenRadar->setScale(1.0);
    scene->addItem(imagenRadar);

    // Crear texto del cronómetro
    textoCronometro = new QGraphicsTextItem();
    textoCronometro->setFont(fuentePersonalizada);
    textoCronometro->setDefaultTextColor(Qt::white);
    textoCronometro->setPos(860, 22); // Posición a la derecha del radar
    scene->addItem(textoCronometro);
    actualizarTextoCronometro();

    // Crear imagen del puntaje
    imagenPuntaje = new QGraphicsPixmapItem(QPixmap(":/Imagenes/Puntaje.png"));
    imagenPuntaje->setPos(940, 28);
    imagenPuntaje->setScale(1.0);
    scene->addItem(imagenPuntaje);

    // Crear texto del puntaje
    textoPuntaje = new QGraphicsTextItem();
    textoPuntaje->setFont(fuentePersonalizada);
    textoPuntaje->setDefaultTextColor(Qt::white);
    textoPuntaje->setPos(975, 22); // Posición a la derecha del icono de puntaje
    scene->addItem(textoPuntaje);
    actualizarTextoPuntaje();
}

void MainWindow::limpiarElementosUI()
{
    // Limpiar elementos del cronómetro
    if (imagenRadar) {
        if (imagenRadar->scene()) {
            scene->removeItem(imagenRadar);
        }
        delete imagenRadar;
        imagenRadar = nullptr;
    }

    if (textoCronometro) {
        if (textoCronometro->scene()) {
            scene->removeItem(textoCronometro);
        }
        textoCronometro->deleteLater();
        textoCronometro = nullptr;
    }

    // Limpiar elementos del puntaje
    if (imagenPuntaje) {
        if (imagenPuntaje->scene()) {
            scene->removeItem(imagenPuntaje);
        }
        delete imagenPuntaje;
        imagenPuntaje = nullptr;
    }

    if (textoPuntaje) {
        if (textoPuntaje->scene()) {
            scene->removeItem(textoPuntaje);
        }
        textoPuntaje->deleteLater();
        textoPuntaje = nullptr;
    }
}

void MainWindow::actualizarTextoCronometro()
{
    if (textoCronometro) {
        QString tiempo = QString::number(tiempoRestante);
        textoCronometro->setPlainText(tiempo);
    }
}

void MainWindow::actualizarTextoPuntaje()
{
    if (textoPuntaje) {
        QString puntaje = QString::number(puntuacion);
        textoPuntaje->setPlainText(puntaje);
    }
} 


void MainWindow::Inicio()
{
    // Limpiar cualquier estado previo
    limpiarTodo();
    juegoReiniciado = false;
    QMediaPlaylist *videoPlaylist = new QMediaPlaylist(this);
    videoPlaylist->addMedia(QUrl("qrc:/Videos/inicio.mp4"));
    videoPlaylist->setPlaybackMode(QMediaPlaylist::Loop);
    videoPlayer = new QMediaPlayer(this);
    videoPlayer->setPlaylist(videoPlaylist);
    videoPlayer->setVolume(0);

    videoWidget = new QVideoWidget(this);
    videoWidget->setGeometry(0, 0, 1200, 675);
    videoWidget->setStyleSheet("background-color: black;");
    videoPlayer->setVideoOutput(videoWidget);
    videoWidget->show();
    videoPlayer->play();

    QMediaPlaylist *audioPlaylist = new QMediaPlaylist(this);
    audioPlaylist->addMedia(QUrl("qrc:/Audios/opening.mp3"));
    audioPlaylist->setPlaybackMode(QMediaPlaylist::Loop);

    audioPlayer = new QMediaPlayer(this);
    audioPlayer->setPlaylist(audioPlaylist);
    audioPlayer->setVolume(100);
    audioPlayer->play();

    // Crear botones con sistema de bloqueo
    addButtonWithLock(":/Imagenes/BotonLvl1.png", ":/Imagenes/BotonLvl1M.png",
                      "", QPoint(100, 500), 1, [this]() {
                          limpiarMenu();
                          Nivel1();
                      });

    addButtonWithLock(":/Imagenes/BotonLvl2.png", ":/Imagenes/BotonLvl2M.png",
                      ":/Imagenes/BotonLvl2B.png", QPoint(550, 500), 2, [this]() {
                          limpiarMenu();
                          Nivel2();
                      });

    addButtonWithLock(":/Imagenes/BotonLvl3.png", ":/Imagenes/BotonLvl3M.png",
                      ":/Imagenes/BotonLvl3B.png", QPoint(1000, 525), 3, [this]() {
                          limpiarMenu();
                          Nivel3();
                      });

    addTransparentButton(":/Imagenes/Volver.png", ":/Imagenes/VolverM.png", QPoint(1115, 22), []() {
        qDebug() << "Saliendo del juego...";
        qApp->quit();
    });
}

void MainWindow::limpiarMenu()
{
    // Limpiar botones del menú
    for (QPushButton *btn : menuButtons) {
        btn->hide();
        btn->deleteLater();
    }
    menuButtons.clear();

    // Limpiar reproductores de media
    if (videoPlayer) {
        videoPlayer->stop();
        videoPlayer->deleteLater();
        videoPlayer = nullptr;
    }
    if (audioPlayer) {
        audioPlayer->stop();
        audioPlayer->deleteLater();
        audioPlayer = nullptr;
    }
    if (videoWidget) {
        videoWidget->hide();
        videoWidget->deleteLater();
        videoWidget = nullptr;
    }
}

void MainWindow::limpiarTodo()
{
    // Detener y limpiar timers PRIMERO
    if (timerParabola) {
        timerParabola->stop();
        timerParabola->deleteLater();
        timerParabola = nullptr;
    }

    if (timerCronometro) {
        timerCronometro->stop();
        timerCronometro->deleteLater();
        timerCronometro = nullptr;
    }

    // Limpiar objetos del juego ANTES de limpiar la escena
    if (scene) {
        // Desconectar todas las señales antes de eliminar objetos
        disconnect(this, nullptr, nullptr, nullptr);

        // Limpiar elementos UI
        limpiarElementosUI();

        // Limpiar saibaman
        if (saibaman) {
            if (saibaman->scene()) {
                scene->removeItem(saibaman);
            }
            saibaman->deleteLater();
            saibaman = nullptr;
        }

        // Limpiar goku
        if (goku) {
            if (goku->scene()) {
                scene->removeItem(goku);
            }
            goku->deleteLater();
            goku = nullptr;
        }

        // Limpiar indicador de vida
        if (indicadorVida) {
            if (indicadorVida->scene()) {
                scene->removeItem(indicadorVida);
            }
            indicadorVida->deleteLater();
            indicadorVida = nullptr;
        }

        // Limpiar línea de parábola
        if (lineaParabola) {
            if (lineaParabola->scene()) {
                scene->removeItem(lineaParabola);
            }
            delete lineaParabola;
            lineaParabola = nullptr;
        }

        // Limpiar la escena completa DESPUÉS de remover todos los items
        scene->clear();
        scene->deleteLater();
        scene = nullptr;
    }

    // Limpiar vista DESPUÉS de limpiar la escena
    if (vista) {
        vista->setScene(nullptr);
        vista->hide();
        vista->deleteLater();
        vista = nullptr;
    }

    // Limpiar botones del menú
    limpiarMenu();

    // Resetear variables de control
    controlesActivos = false;
    teclasPresionadas.clear();
    contadorEspacio = 0;
    anguloFijado = false;
    velocidadFijada = false;
    aumentandoAngulo = true;
    aumentandoVelocidad = true;
    permitirMovimientoVertical = false;
}

void MainWindow::agregarBotonVolver()
{
    addTransparentButton(":/Imagenes/Volver.png", ":/Imagenes/VolverM.png", QPoint(1115, 22), [this]() {
        limpiarTodo();
        Inicio();
    });
}

void MainWindow::Nivel1()
{
    // Inicializar valores del nivel 1
    puntuacion = 0;
    tiempoRestante = 60;

    vista = new QGraphicsView(this);
    vista->setGeometry(0, 0, 1200, 675);
    vista->show();

    scene = new QGraphicsScene(this);
    vista->setScene(scene);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFixedSize(1200, 675);

    QGraphicsPixmapItem *fondo = new QGraphicsPixmapItem(QPixmap(":/Imagenes/FondoNivel1.png"));
    scene->addItem(fondo);

    // Crear elementos de UI (cronómetro y puntaje)
    crearElementosUI();

    agregarBotonVolver();
    goku = new Personaje(":/Imagenes/GokuSpriteLvl1.png", 63, 69, 4, 1, Personaje::PorEvento);
    scene->addItem(goku);
    goku->setPos(10, 100);
    goku->setScale(3);

    // AGREGAR SAIBAMAN
    saibaman = new Saibaman();
    scene->addItem(saibaman);
    saibaman->setScale(3);

    controlesActivos = false;
    contadorEspacio = 0;

    angulo = 10.0;
    velocidad = 70.0;
    anguloFijado = false;
    velocidadFijada = false;
    aumentandoAngulo = true;
    aumentandoVelocidad = true;

    lineaParabola = new QGraphicsPathItem();
    QPen pen(Qt::yellow);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    lineaParabola->setPen(pen);
    scene->addItem(lineaParabola);

    timerParabola = new QTimer(this);
    connect(timerParabola, &QTimer::timeout, this, &MainWindow::actualizarParabola);
    timerParabola->start(100);

    indicadorVida = new IndicadorVida();
    scene->addItem(indicadorVida);
    indicadorVida->setPos(0, 529);
    indicadorVida->setScale(1.5);
    indicadorVida->setVida(vida);

    // Inicializar cronómetro
    timerCronometro = new QTimer(this);
    connect(timerCronometro, &QTimer::timeout, this, &MainWindow::actualizarCronometro);
    timerCronometro->start(1000); // 1 segundo
}

void MainWindow::actualizarCronometro()
{
    tiempoRestante--;
    actualizarTextoCronometro(); // Actualizar el texto visual

    if (tiempoRestante <= 0) {
        // Tiempo terminado
        if (timerCronometro) {
            timerCronometro->stop();
        }

        // Verificar si ganó el nivel (puntuación >= 10)
        if (puntuacion >= 10) {
            puntuacionTotal = puntuacion; // Guardar puntuación para nivel 2
            qDebug() << "¡Nivel completado! Puntuación:" << puntuacion;

            // DESBLOQUEAR NIVEL 2 al ganar el nivel 1
            if (nivelMaximoDesbloqueado < 2) {
                nivelMaximoDesbloqueado = 2;
                qDebug() << "¡Nivel 2 desbloqueado!";
            }

        } else {
            puntuacion = 0; // Reiniciar puntuación si no alcanzó el mínimo
            qDebug() << "Tiempo terminado. Puntuación insuficiente.";
        }

        // Volver al menú principal
        vida = 3;
        juegoReiniciado = true;
        QTimer::singleShot(0, this, [this]() {
            limpiarTodo();
            Inicio();
        });
    }
}

void MainWindow::crearNuevoSaibaman()
{
    if (saibaman && scene) {
        scene->removeItem(saibaman);
        saibaman->deleteLater();
    }

    saibaman = new Saibaman();
    scene->addItem(saibaman);
    saibaman->setScale(3);
}

void MainWindow::actualizarParabola()
{
    if (!anguloFijado) {
        if (angulo >= 60.0) aumentandoAngulo = false;
        if (angulo <= 10.0) aumentandoAngulo = true;
        angulo += aumentandoAngulo ? 2.0 : -2.0;
    } else if (!velocidadFijada) {
        if (velocidad >= 80.0) aumentandoVelocidad = false;
        if (velocidad <= 30.0) aumentandoVelocidad = true;
        velocidad += aumentandoVelocidad ? 2.0 : -2.0;
    }

    if (lineaParabola && scene) {
        scene->removeItem(lineaParabola);
        delete lineaParabola;
        lineaParabola = nullptr;
    }

    QPainterPath path;
    QPointF origen(148, 153);
    double g = 9.8;
    double rad = qDegreesToRadians(angulo);
    double vx = velocidad * std::cos(rad);
    double vy = -velocidad * std::sin(rad);
    double t = 0.0;
    double dt = 0.1;

    path.moveTo(origen);

    while (true) {
        double x = vx * t;
        double y = vy * t + 0.5 * g * t * t;
        QPointF punto(origen.x() + x, origen.y() + y);
        if (punto.y() >= 500) break;
        path.lineTo(punto);
        t += dt;
    }

    lineaParabola = scene->addPath(path, QPen(Qt::yellow, 2, Qt::DashLine));
}

void MainWindow::manejarFinAnimacionEvento()
{
    if (juegoReiniciado) {
        qDebug() << "Evento cancelado porque el juego fue reiniciado.";
        return;
    }

    // Verificar que todos los objetos necesarios existan
    if (!scene || !goku || !saibaman) {
        qDebug() << "Objetos no disponibles. Cancelando creación de bala.";
        return;
    }

    // Verificar que los objetos estén en la escena
    if (goku->scene() != scene || saibaman->scene() != scene) {
        qDebug() << "Objetos no están en la escena actual. Cancelando.";
        return;
    }

    if (lineaParabola) lineaParabola->hide();

    QPointF origen(148, 153);
    Bala *bala = new Bala(origen, anguloBala, velocidadBala);
    if (!bala) {
        qDebug() << "No se pudo crear la bala.";
        return;
    }

    scene->addItem(bala);

    // Usar QPointer para detectar si los objetos son eliminados
    QPointer<MainWindow> thisPtr(this);
    QPointer<QGraphicsScene> scenePtr(scene);

    connect(bala, QOverload<Saibaman*>::of(&Bala::balaColisionoSaibaman), this,
            [this, thisPtr, scenePtr](Saibaman* saibaman) {
                if (!thisPtr || !scenePtr) return;  // Verificar que los objetos existan

                if (saibaman && saibaman->scene() == scenePtr) {
                    // Iniciar animación de muerte en lugar de eliminar inmediatamente
                    saibaman->iniciarAnimacionMuerte();

                    // Conectar señal para cuando termine la animación de muerte
                    connect(saibaman, &Saibaman::animacionMuerteCompleta, this, [this, saibaman]() {
                        if (scene && saibaman && saibaman->scene() == scene) {
                            scene->removeItem(saibaman);
                            saibaman->deleteLater();

                            // Incrementar puntuación
                            puntuacion += 3;
                            actualizarTextoPuntaje();
                            qDebug() << "Puntuación actual:" << puntuacion;

                            // Crear nuevo Saibaman
                            crearNuevoSaibaman();
                        }
                    });
                }
                resetearEstadoJuego();
            });

    connect(bala, &Bala::balaPerdida, this, [this, thisPtr]() {
        if (!thisPtr) return;  // Verificar que MainWindow exista

        vida--;
        if (indicadorVida) {
            indicadorVida->setVida(vida);
        }

        if (vida <= 0) {
            if (timerCronometro) {
                timerCronometro->stop();
            }

            // Al perder en nivel 1, puntuación se reinicia a 0
            puntuacion = 0;
            vida = 3;
            juegoReiniciado = true;
            QTimer::singleShot(0, this, [this]() {
                limpiarTodo();
                Inicio();
            });
            return;
        }

        resetearEstadoJuego();
    });

    connect(bala, &Bala::balaDestruida, this, [this, thisPtr]() {
        if (!thisPtr) return;  // Verificar que MainWindow exista
        resetearEstadoJuego();
    });
}

void MainWindow::resetearEstadoJuego()
{
    if (goku) {
        goku->resetearAlPrimerFrame();
    }

    angulo = 10.0;
    velocidad = 70.0;
    anguloFijado = false;
    velocidadFijada = false;
    aumentandoAngulo = true;
    aumentandoVelocidad = true;
    if (lineaParabola) lineaParabola->show();
    if (timerParabola) timerParabola->start(100);
    contadorEspacio = 0;
}

void MainWindow::Nivel2()
{
    // DESBLOQUEAR NIVEL 3 al entrar al nivel 2
    if (nivelMaximoDesbloqueado < 3) {
        nivelMaximoDesbloqueado = 3;
        qDebug() << "¡Nivel 3 desbloqueado!";
    }

    // Inicializar puntuación del nivel 2 con la puntuación guardada del nivel 1
    puntuacion = puntuacionTotal;

    vista = new QGraphicsView(this);
    vista->setGeometry(0, 0, 1200, 675);
    vista->show();

    scene = new QGraphicsScene(this);
    vista->setScene(scene);

    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFixedSize(1200, 675);

    QGraphicsPixmapItem *fondo = new QGraphicsPixmapItem(QPixmap(":/Imagenes/FondoNivel2.png"));
    scene->addItem(fondo);

    agregarBotonVolver();

    goku = new Personaje(":/Imagenes/GokuSprite.png", 61, 62, 3, 4, Personaje::Permanente);
    scene->addItem(goku);
    goku->setPos(400, 400);
    goku->setScale(3);

    controlesActivos = true;
    permitirMovimientoVertical = false;

    indicadorVida = new IndicadorVida();
    scene->addItem(indicadorVida);
    indicadorVida->setPos(0, 529);
    indicadorVida->setScale(1.5);
    indicadorVida->setVida(vida);
}

void MainWindow::Nivel3()
{
    puntuacion = puntuacionTotal;

    vista = new QGraphicsView(this);
    vista->setGeometry(0, 0, 1200, 675);
    vista->show();

    scene = new QGraphicsScene(this);
    vista->setScene(scene);

    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFixedSize(1200, 675);

    QGraphicsPixmapItem *fondo = new QGraphicsPixmapItem(QPixmap(":/Imagenes/FondoNivel3.png"));
    scene->addItem(fondo);

    agregarBotonVolver();

    goku = new Personaje(":/Imagenes/GokuSprite.png", 61, 62, 3, 4, Personaje::Permanente);
    scene->addItem(goku);
    goku->setPos(400, 300);
    goku->setScale(3);

    controlesActivos = true;
    permitirMovimientoVertical = true;

    indicadorVida = new IndicadorVida();
    scene->addItem(indicadorVida);
    indicadorVida->setPos(0, 529);
    indicadorVida->setScale(1.5);
    indicadorVida->setVida(vida);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!goku) return;

    int key = event->key();

    if (vista && vista->scene() == scene && scene && goku->scene() == scene) {
        if (key == Qt::Key_Space) {
            contadorEspacio++;
            if (contadorEspacio == 1) {
                anguloFijado = true;
                anguloBala = angulo;
            } else if (contadorEspacio == 2) {
                velocidadFijada = true;
                velocidadBala = velocidad;
                if (timerParabola) timerParabola->stop();
                if (lineaParabola) lineaParabola->hide();
                goku->iniciarAnimacionEvento();

                // Usar QPointer para verificar que MainWindow exista cuando se ejecute
                QPointer<MainWindow> thisPtr(this);
                QTimer::singleShot(1000, this, [this, thisPtr]() {
                    if (thisPtr) {  // Verificar que MainWindow aún exista
                        manejarFinAnimacionEvento();
                    }
                });
            }
            return;
        }
    }

    if (!controlesActivos) return;
    if (teclasPresionadas.contains(key)) return;

    teclasPresionadas.insert(key);

    switch (key) {
    case Qt::Key_W:
        if (permitirMovimientoVertical) goku->moverseArriba();
        break;
    case Qt::Key_S:
        if (permitirMovimientoVertical) goku->moverseAbajo();
        break;
    case Qt::Key_A:
        goku->moverseIzquierda();
        break;
    case Qt::Key_D:
        goku->moverseDerecha();
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!controlesActivos || !goku) return;

    int key = event->key();
    teclasPresionadas.remove(key);

    if (key == Qt::Key_A || key == Qt::Key_D) {
        goku->detenerMovimiento();
    }
}

void MainWindow::addTransparentButton(const QString &imagenNormal, const QString &imagenHover, const QPoint &posicion, std::function<void()> onClick)
{
    QPixmap pixmapNormal(imagenNormal);
    QPixmap pixmapHover(imagenHover);

    QPushButton *boton = new QPushButton(this);
    boton->setIcon(QIcon(pixmapNormal));
    boton->setIconSize(pixmapNormal.size());
    boton->setFlat(true);
    boton->setCursor(Qt::PointingHandCursor);
    boton->setStyleSheet("background-color: transparent;");
    boton->setGeometry(posicion.x(), posicion.y(), pixmapNormal.width(), pixmapNormal.height());
    boton->setMask(pixmapNormal.createMaskFromColor(Qt::transparent, Qt::MaskInColor));

    boton->setFocusPolicy(Qt::NoFocus);

    boton->installEventFilter(new HoverIconEventFilter(pixmapNormal, pixmapHover, boton));
    connect(boton, &QPushButton::clicked, this, [onClick]() { onClick(); });
    boton->show();
}

void MainWindow::addButtonWithLock(const QString &imgNormal, const QString &imgHover,
                                   const QString &imgBlocked, const QPoint &pos,
                                   int nivelRequerido, std::function<void()> cb)
{
    bool isBlocked = nivelMaximoDesbloqueado < nivelRequerido;

    QPixmap pixmapNormal(imgNormal);
    QPixmap pixmapHover(imgHover);
    QPixmap pixmapBlocked;

    if (!imgBlocked.isEmpty()) {
        pixmapBlocked = QPixmap(imgBlocked);
    }

    QPushButton *boton = new QPushButton(this);

    // Configurar el botón según si está bloqueado o no
    if (isBlocked && !imgBlocked.isEmpty()) {
        boton->setIcon(QIcon(pixmapBlocked));
        boton->setIconSize(pixmapBlocked.size());
        boton->setGeometry(pos.x(), pos.y(), pixmapBlocked.width(), pixmapBlocked.height());
        boton->setMask(pixmapBlocked.createMaskFromColor(Qt::transparent, Qt::MaskInColor));
    } else {
        boton->setIcon(QIcon(pixmapNormal));
        boton->setIconSize(pixmapNormal.size());
        boton->setGeometry(pos.x(), pos.y(), pixmapNormal.width(), pixmapNormal.height());
        boton->setMask(pixmapNormal.createMaskFromColor(Qt::transparent, Qt::MaskInColor));
    }

    boton->setFlat(true);
    boton->setStyleSheet("background-color: transparent;");

    boton->setFocusPolicy(Qt::NoFocus);

    if (!isBlocked) {
        boton->setCursor(Qt::PointingHandCursor);
        if (!imgBlocked.isEmpty()) {
            boton->installEventFilter(new HoverIconEventFilterWithLock(pixmapNormal, pixmapHover, pixmapBlocked, boton, false));
        } else {
            boton->installEventFilter(new HoverIconEventFilter(pixmapNormal, pixmapHover, boton));
        }
        connect(boton, &QPushButton::clicked, this, [cb]() { cb(); });
    } else {
        boton->setCursor(Qt::ArrowCursor);
        if (!imgBlocked.isEmpty()) {
            boton->installEventFilter(new HoverIconEventFilterWithLock(pixmapNormal, pixmapHover, pixmapBlocked, boton, true));
        }
    }

    boton->show();
    menuButtons.append(boton);
}

MainWindow::~MainWindow()
{
    // Detener cualquier timer activo ANTES de limpiar
    if (timerParabola) {
        timerParabola->stop();
    }
    if (timerCronometro) {
        timerCronometro->stop();
    }

    // Desconectar todas las señales
    disconnect(this, nullptr, nullptr, nullptr);

    limpiarTodo();
    delete ui;
}
