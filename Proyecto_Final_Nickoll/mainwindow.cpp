#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bala.h"
#include "indicadorvida.h"
#include "basura.h"
#include <QKeyEvent>
#include <QMediaPlaylist>
#include <QPixmap>
#include <QBitmap>
#include <QApplication>
#include <QDebug>
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <algorithm>
#include <QVector>

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
    : QMainWindow(parent), ui(new Ui::MainWindow), vida(3), puntuacion(0),
    puntuacionTotal(0), tiempoRestante(60), puntuacionNivel1Ganada(0), puntuacionNivel2Ganada(0)
{

    ui->setupUi(this);
    this->resize(1200, 675);

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
    timerGenerarBasura = nullptr;
    timerColisiones = nullptr;

    // Inicializar nuevos elementos UI
    imagenRadar = nullptr;
    textoCronometro = nullptr;
    imagenPuntaje = nullptr;
    textoPuntaje = nullptr;
    timerGenerarBasura4 = nullptr;

    imagenEsferas = nullptr;
    textoEsferas = nullptr;

    // Inicializar timer y fondo del nivel 3
    timerFondoNivel3 = nullptr;
    fondoNivel3 = nullptr;

    audioNivel = nullptr;
    audioDisparo = nullptr;

    timerFisicasViento = nullptr;
    velocidadViento = 1.0;        // Velocidad del viento
    fuerzaViento = 0.0;
    resistenciaAire = 0.95;       // Factor de resistencia (0.95 = 5% de resistencia)
    velocidadGokuX = 0.0;
    velocidadGokuY = 0.0;
    aceleracionViento = 0.15;     // Aceleración por el viento
    vientoActivo = false;

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

void MainWindow::crearElementosUI(int nivel)
{
    if (!scene) return;

    if (nivel == 2) {
        imagenEsferas = new QGraphicsPixmapItem(QPixmap(":/Imagenes/esfera.png"));
        imagenEsferas->setPos(710, 13);
        imagenEsferas->setScale(1.0);
        scene->addItem(imagenEsferas);

        // Crear texto de las esferas
        textoEsferas = new QGraphicsTextItem();
        textoEsferas->setFont(fuentePersonalizada);
        textoEsferas->setDefaultTextColor(Qt::white);
        textoEsferas->setPos(760, 22);
        scene->addItem(textoEsferas);
        actualizarTextoEsferas();
    }

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
    textoPuntaje->setPos(975, 22);
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
    if (imagenEsferas) {
        if (imagenEsferas->scene()) {
            scene->removeItem(imagenEsferas);
        }
        delete imagenEsferas;
        imagenEsferas = nullptr;
    }

    if (textoEsferas) {
        if (textoEsferas->scene()) {
            scene->removeItem(textoEsferas);
        }
        textoEsferas->deleteLater();
        textoEsferas = nullptr;
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
        reproducirSonidoEfecto("qrc:/Audios/Victoria.mp3");
    }
}

void MainWindow::actualizarTextoEsferas()
{
    if (textoEsferas) {
        QString esfera = QString::number(esferas);
        textoEsferas->setPlainText(esfera);
    }
}

void MainWindow::Inicio()
{
    vida = 3;
    // Limpiar cualquier estado previo
    limpiarTodo();
    juegoReiniciado = false;

    // Configurar audio de fondo
    reproducirAudioNivel("qrc:/Audios/opening.mp3", true);

    // Configurar video de fondo
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

    // Crear overlay semi-transparente para los botones
    QWidget *overlay = new QWidget(this);
    overlay->setGeometry(0, 490, 1200, 205);
    overlay->setStyleSheet("background-color: rgba(107, 35, 30, 255);");
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlay->show();

    // Crear panel de puntuaciones en la parte izquierda
    crearPanelPuntuaciones();

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

void MainWindow::crearPanelPuntuaciones()
{
    // Cargar puntuaciones desde archivo
    auto puntuaciones = cargarPuntuacionesDesdeArchivo();

    // Crear widget contenedor para el panel de puntuaciones
    panelPuntuaciones = new QWidget(this);
    panelPuntuaciones->setGeometry(0, 0, 320, 490);
    panelPuntuaciones->setStyleSheet("background-color: rgba(107, 35, 30, 255);");
    panelPuntuaciones->show();

    // Layout principal del panel
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(panelPuntuaciones);
    layoutPrincipal->setContentsMargins(15, 15, 15, 15);
    layoutPrincipal->setSpacing(15);

    // Título del panel
    QLabel *titulo = new QLabel("PUNTUACIONES", panelPuntuaciones);
    titulo->setFont(QFont(fuentePersonalizada.family(), 20, QFont::Bold));
    titulo->setStyleSheet("color: #FFD700; text-align: center;");
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setMinimumHeight(35);
    layoutPrincipal->addWidget(titulo);

    // Separador
    QFrame *separador1 = new QFrame(panelPuntuaciones);
    separador1->setFrameShape(QFrame::HLine);
    separador1->setStyleSheet("color: #FFD700;");
    layoutPrincipal->addWidget(separador1);

    // Sección de puntuaciones recientes
    QLabel *tituloRecientes = new QLabel("RECIENTES", panelPuntuaciones);
    tituloRecientes->setFont(QFont(fuentePersonalizada.family(), 16, QFont::Bold));
    tituloRecientes->setStyleSheet("color: #87CEEB; text-align: center;");
    tituloRecientes->setAlignment(Qt::AlignCenter);
    tituloRecientes->setMinimumHeight(30);
    layoutPrincipal->addWidget(tituloRecientes);

    // Mostrar últimas 3 puntuaciones
    auto recientes = obtenerUltimasPuntuaciones(puntuaciones, 3);
    for (const auto &entrada : recientes) {
        QLabel *labelFecha = new QLabel(entrada.fecha.toString("dd/MM/yyyy"), panelPuntuaciones);
        labelFecha->setFont(QFont(fuentePersonalizada.family(), 11));
        labelFecha->setStyleSheet("color: #FFFFFF; padding: 3px;");
        labelFecha->setAlignment(Qt::AlignCenter);
        labelFecha->setMinimumHeight(28);

        QLabel *labelPuntuacion = new QLabel(QString("Puntos: %1").arg(entrada.puntuacion), panelPuntuaciones);
        labelPuntuacion->setFont(QFont(fuentePersonalizada.family(), 13, QFont::Bold));
        labelPuntuacion->setStyleSheet("color: #90EE90; padding: 5px;");
        labelPuntuacion->setAlignment(Qt::AlignCenter);
        labelPuntuacion->setMinimumHeight(30);

        layoutPrincipal->addWidget(labelFecha);
        layoutPrincipal->addWidget(labelPuntuacion);

        // Espaciado entre entradas
        layoutPrincipal->addSpacing(8);
    }

    // Separador
    QFrame *separador2 = new QFrame(panelPuntuaciones);
    separador2->setFrameShape(QFrame::HLine);
    separador2->setStyleSheet("color: #FFD700;");
    layoutPrincipal->addWidget(separador2);

    // Sección de mejores puntuaciones
    QLabel *tituloMejores = new QLabel("MEJORES", panelPuntuaciones);
    tituloMejores->setFont(QFont(fuentePersonalizada.family(), 16, QFont::Bold));
    tituloMejores->setStyleSheet("color: #FFB6C1; text-align: center;");
    tituloMejores->setAlignment(Qt::AlignCenter);
    tituloMejores->setMinimumHeight(30);
    layoutPrincipal->addWidget(tituloMejores);

    // Mostrar mejores 3 puntuaciones
    auto mejores = obtenerMejoresPuntuaciones(puntuaciones, 3);
    int posicion = 1;
    for (const auto &entrada : mejores) {
        QString colorPosicion;
        QString simbolo;

        switch (posicion) {
        case 1:
            colorPosicion = "#FFD700";
            simbolo = "🥇";
            break;
        case 2:
            colorPosicion = "#C0C0C0";
            simbolo = "🥈";
            break;
        case 3:
            colorPosicion = "#CD7F32";
            simbolo = "🥉";
            break;
        }

        QLabel *labelPosicion = new QLabel(QString("%1 %2° Lugar").arg(simbolo).arg(posicion), panelPuntuaciones);
        labelPosicion->setFont(QFont(fuentePersonalizada.family(), 11));
        labelPosicion->setStyleSheet(QString("color: %1; padding: 3px;").arg(colorPosicion));
        labelPosicion->setAlignment(Qt::AlignCenter);
        labelPosicion->setMinimumHeight(28);

        QLabel *labelPuntuacion = new QLabel(QString("Puntos: %1").arg(entrada.puntuacion), panelPuntuaciones);
        labelPuntuacion->setFont(QFont(fuentePersonalizada.family(), 13, QFont::Bold));
        labelPuntuacion->setStyleSheet("color: #FFA500; padding: 5px;");
        labelPuntuacion->setAlignment(Qt::AlignCenter);
        labelPuntuacion->setMinimumHeight(30);

        layoutPrincipal->addWidget(labelPosicion);
        layoutPrincipal->addWidget(labelPuntuacion);

        posicion++;
        layoutPrincipal->addSpacing(6);
    }

    layoutPrincipal->addStretch();
}

QVector<MainWindow::EntradaPuntuacion> MainWindow::cargarPuntuacionesDesdeArchivo()
{
    QVector<MainWindow::EntradaPuntuacion> puntuaciones;
    QString nombreArchivo = "puntuaciones_guardadas.txt";
    QFile archivo(nombreArchivo);

    if (!archivo.exists()) {
        return puntuaciones;
    }

    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            QString linea = in.readLine();
            QStringList partes = linea.split(" - Puntuación: ");
            if (partes.size() == 2) {
                QDateTime fecha = QDateTime::fromString(partes[0], "yyyy-MM-dd hh:mm:ss");
                int puntuacion = partes[1].toInt();

                if (fecha.isValid() && puntuacion > 0) {
                    MainWindow::EntradaPuntuacion entrada;
                    entrada.fecha = fecha;
                    entrada.puntuacion = puntuacion;
                    puntuaciones.append(entrada);
                }
            }
        }
        archivo.close();
    }

    return puntuaciones;
}

QVector<MainWindow::EntradaPuntuacion> MainWindow::obtenerUltimasPuntuaciones(const QVector<MainWindow::EntradaPuntuacion> &puntuaciones, int cantidad)
{
    QVector<MainWindow::EntradaPuntuacion> ordenadas = puntuaciones;

    // Ordenar por fecha (más reciente primero)
    std::sort(ordenadas.begin(), ordenadas.end(), [](const MainWindow::EntradaPuntuacion &a, const MainWindow::EntradaPuntuacion &b) {
        return a.fecha > b.fecha;
    });

    // Retornar solo la cantidad solicitada
    QVector<MainWindow::EntradaPuntuacion> resultado;
    for (int i = 0; i < qMin(cantidad, ordenadas.size()); i++) {
        resultado.append(ordenadas[i]);
    }

    return resultado;
}

void MainWindow::limpiarPanelPuntuaciones()
{
    if (panelPuntuaciones) {
        panelPuntuaciones->hide();
        panelPuntuaciones->deleteLater();
        panelPuntuaciones = nullptr;
    }
}

QVector<MainWindow::EntradaPuntuacion> MainWindow::obtenerMejoresPuntuaciones(const QVector<MainWindow::EntradaPuntuacion> &puntuaciones, int cantidad)
{
    QVector<MainWindow::EntradaPuntuacion> ordenadas = puntuaciones;

    // Ordenar por puntuación (mayor a menor)
    std::sort(ordenadas.begin(), ordenadas.end(), [](const MainWindow::EntradaPuntuacion &a, const MainWindow::EntradaPuntuacion &b) {
        return a.puntuacion > b.puntuacion;
    });

    // Retornar solo la cantidad solicitada
    QVector<MainWindow::EntradaPuntuacion> resultado;
    for (int i = 0; i < qMin(cantidad, ordenadas.size()); i++) {
        resultado.append(ordenadas[i]);
    }

    return resultado;
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
    detenerFisicasViento();

    limpiarPanelPuntuaciones();

    if (audioNivel) {
        audioNivel->stop();
        audioNivel->deleteLater();
        audioNivel = nullptr;
    }

    if (audioDisparo) {
        audioDisparo->stop();
        audioDisparo->deleteLater();
        audioDisparo = nullptr;
    }

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

    // Limpiar timer de fondo nivel 3
    if (timerFondoNivel3) {
        timerFondoNivel3->stop();
        timerFondoNivel3->deleteLater();
        timerFondoNivel3 = nullptr;
    }

    // Limpiar timer de basura4
    if (timerGenerarBasura4) {
        timerGenerarBasura4->stop();
        timerGenerarBasura4->deleteLater();
        timerGenerarBasura4 = nullptr;
    }

    limpiarBasura();

    // Limpiar objetos del juego ANTES de limpiar la escena
    if (scene) {
        // Desconectar todas las señales antes de eliminar objetos
        disconnect(this, nullptr, nullptr, nullptr);

        // Limpiar elementos UI
        limpiarElementosUI();

        // Limpiar fondo nivel 3
        if (fondoNivel3) {
            if (fondoNivel3->scene()) {
                scene->removeItem(fondoNivel3);
            }
            delete fondoNivel3;
            fondoNivel3 = nullptr;
        }

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

void MainWindow::reproducirSonidoEfecto(const QString &archivoAudio)
{
    // Detener audio de disparo anterior si existe
    if (audioDisparo) {
        audioDisparo->stop();
        audioDisparo->deleteLater();
        audioDisparo = nullptr;
    }

    // Crear nuevo reproductor para el sonido de disparo
    audioDisparo = new QMediaPlayer(this);
    audioDisparo->setMedia(QUrl(archivoAudio));
    audioDisparo->setVolume(100);
    audioDisparo->play();

    qDebug() << "Reproduciendo sonido de disparo";
}

void MainWindow::reproducirAudioNivel(const QString &archivoAudio, bool loop)
{
    // Detener audio anterior si existe
    if (audioNivel) {
        audioNivel->stop();
        audioNivel->deleteLater();
        audioNivel = nullptr;
    }

    // Crear nuevo reproductor para el nivel
    audioNivel = new QMediaPlayer(this);
    audioNivel->setMedia(QUrl(archivoAudio));
    audioNivel->setVolume(40);

    if (loop) {
        // Conectar para reiniciar cuando termine
        connect(audioNivel, &QMediaPlayer::mediaStatusChanged, this,
                [this](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::EndOfMedia) {
                        audioNivel->setPosition(0);
                        audioNivel->play();
                    }
                });
    }

    audioNivel->play();
    qDebug() << "Reproduciendo audio de nivel:" << archivoAudio << (loop ? "(loop)" : "");
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
    puntuacionNivel1Ganada = 0;
    puntuacionNivel2Ganada = 0;
    tiempoRestante = 60;
    nivelMaximoDesbloqueado = 1;

    reproducirAudioNivel("qrc:/Audios/Nivel1.mp3");

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
    crearElementosUI(1);

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
    actualizarTextoCronometro();

    if (tiempoRestante <= 0) {
        if (timerCronometro) {
            timerCronometro->stop();
        }

        // Detener físicas de viento
        detenerFisicasViento();

        // Detener timer del fondo nivel 3 cuando termine el tiempo
        if (timerFondoNivel3) {
            timerFondoNivel3->stop();
        }

        // Verificar si ganó el nivel
        if (puntuacion >= 10) {
            // Determinar qué nivel se completó y guardar puntuación
            if (permitirMovimientoVertical) {
                // Nivel 3 completado
                puntuacionTotal = puntuacion;
                guardarPuntuacionConFecha(puntuacionTotal);
                qDebug() << "¡Nivel 3 completado! Puntuación:" << puntuacion;
                nivelMaximoDesbloqueado = 1;
                puntuacion = 0;
            } else if (controlesActivos) {
                if(esferas >= 5){
                    // Nivel 2 completado
                    puntuacionNivel2Ganada = puntuacion;

                    // DESBLOQUEAR NIVEL 3 SOLO CUANDO SE GANE EL NIVEL 2
                    if (nivelMaximoDesbloqueado < 3) {
                        nivelMaximoDesbloqueado = 3;
                        qDebug() << "¡Nivel 3 desbloqueado!";
                    }

                    qDebug() << "¡Nivel 2 completado! Puntuación:" << puntuacion;
                }
                else {
                    qDebug() << "¡Nivel 2 fallido!";
                    esferas=0;
                }
            } else {
                // Nivel 1 completado
                puntuacionNivel1Ganada = puntuacion;
                puntuacionTotal = puntuacion;
                if (nivelMaximoDesbloqueado < 2) {
                    nivelMaximoDesbloqueado = 2;
                    qDebug() << "¡Nivel 2 desbloqueado!";
                }
            }
        } else {
            // No alcanzó el mínimo, restaurar puntuación según el nivel
            if (permitirMovimientoVertical) {
                // Nivel 3: restaurar puntuación del nivel 2
                puntuacion = puntuacionNivel2Ganada;
            } else if (controlesActivos) {
                // Nivel 2: restaurar puntuación del nivel 1
                puntuacion = puntuacionNivel1Ganada;
                qDebug() << "Nivel 2 no completado. Puntuación insuficiente.";
            } else {
                // Nivel 1: puntuación a 0
                puntuacion = 0;
            }
            qDebug() << "Tiempo terminado. Puntuación insuficiente.";
        }

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

    // REPRODUCIR SONIDO DE DISPARO ANTES DE CREAR LA BALA
    reproducirSonidoEfecto("qrc:/Audios/Disparo.mp3");

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
                    reproducirSonidoEfecto("qrc:/Audios/Muerte.mp3");
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
        if((puntuacion-5) < 0){
            puntuacion = 0;
            actualizarTextoPuntaje();
            reproducirSonidoEfecto("qrc:/Audios/VidaMenos.mp3");
        }
        else{
            puntuacion -= 5;
            actualizarTextoPuntaje();
            reproducirSonidoEfecto("qrc:/Audios/VidaMenos.mp3");
        }
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

    // Usar puntuación guardada del nivel 1
    puntuacion = puntuacionNivel1Ganada;
    esferas = 0;
    nivelMaximoDesbloqueado = 2;
    tiempoRestante = 60;

    reproducirAudioNivel("qrc:/Audios/Nivel2.mp3");

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

    // Crear elementos de UI
    crearElementosUI(2);

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
    indicadorVida->setZValue(3);

    // Inicializar cronómetro
    timerCronometro = new QTimer(this);
    connect(timerCronometro, &QTimer::timeout, this, &MainWindow::actualizarCronometro);
    timerCronometro->start(1000);

    // Iniciar generación de basura vertical
    iniciarGeneracionBasura(Basura::Vertical);
}

void MainWindow::Nivel3()
{
    // Usar puntuación guardada del nivel 2
    puntuacion = puntuacionNivel2Ganada;
    tiempoRestante = 60;

    reproducirAudioNivel("qrc:/Audios/Nivel3.mp3");

    vista = new QGraphicsView(this);
    vista->setGeometry(0, 0, 1200, 675);
    vista->show();

    scene = new QGraphicsScene(this);
    vista->setScene(scene);

    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFixedSize(1200, 675);

    fondoNivel3 = new QGraphicsPixmapItem(QPixmap(":/Imagenes/FondoNivel3.png"));
    scene->addItem(fondoNivel3);
    fondoNivel3->setZValue(-1);

    // Crear elementos de UI
    crearElementosUI(3);

    agregarBotonVolver();

    goku = new Personaje(":/Imagenes/GokuSprite.png", 61, 62, 3, 4, Personaje::Permanente);
    scene->addItem(goku);
    goku->setPos(400, 300);
    goku->setScale(3);

    controlesActivos = true;
    permitirMovimientoVertical = true;

    // INICIAR FÍSICAS DE VIENTO
    iniciarFisicasViento();

    indicadorVida = new IndicadorVida();
    scene->addItem(indicadorVida);
    indicadorVida->setPos(0, 529);
    indicadorVida->setScale(1.5);
    indicadorVida->setVida(vida);
    indicadorVida->setZValue(3);

    // Inicializar cronómetro
    timerCronometro = new QTimer(this);
    connect(timerCronometro, &QTimer::timeout, this, &MainWindow::actualizarCronometro);
    timerCronometro->start(1000);

    timerFondoNivel3 = new QTimer(this);
    connect(timerFondoNivel3, &QTimer::timeout, this, &MainWindow::moverFondoNivel3);
    timerFondoNivel3->start(30); // Cada 30ms, como una animación fluida

    // Iniciar generación de basura horizontal
    iniciarGeneracionBasura(Basura::Horizontal);
}

void MainWindow::moverFondoNivel3()
{
    if (!fondoNivel3) return;

    // Mover el fondo hacia la izquierda
    QPointF pos = fondoNivel3->pos();
    pos.setX(pos.x() - 1); // Velocidad del viento, puedes ajustar a -2 o -0.5
    fondoNivel3->setPos(pos);

    // Si el fondo se ha salido totalmente, reiniciar su posición (para hacerlo infinito)
    if (pos.x() <= -fondoNivel3->pixmap().width()) {
        fondoNivel3->setPos(0, pos.y());
    }
}

void MainWindow::iniciarFisicasViento()
{
    if (!goku) return;

    vientoActivo = true;
    velocidadGokuX = 0.0;
    velocidadGokuY = 0.0;

    // Timer para aplicar físicas cada 16ms (~60 FPS)
    timerFisicasViento = new QTimer(this);
    connect(timerFisicasViento, &QTimer::timeout, this, &MainWindow::aplicarFisicasViento);
    timerFisicasViento->start(16);

    qDebug() << "Físicas de viento iniciadas";
}

void MainWindow::detenerFisicasViento()
{
    if (timerFisicasViento) {
        timerFisicasViento->stop();
        timerFisicasViento->deleteLater();
        timerFisicasViento = nullptr;
    }

    vientoActivo = false;
    velocidadGokuX = 0.0;
    velocidadGokuY = 0.0;

    qDebug() << "Físicas de viento detenidas";
}

void MainWindow::aplicarFisicasViento()
{
    if (!goku || !vientoActivo) return;

    // Calcular y aplicar fuerzas
    calcularFuerzaViento();
    aplicarResistenciaAire();
    actualizarPosicionGoku();
    verificarLimitesGoku();
}

void MainWindow::calcularFuerzaViento()
{
    // Fuerza del viento basada en la velocidad del fondo
    fuerzaViento = velocidadViento * aceleracionViento;

    // Aplicar fuerza a la velocidad de Goku
    velocidadGokuX -= fuerzaViento; // Empujar hacia la izquierda

    // Limitar velocidad máxima del viento
    if (velocidadGokuX < -5.0) {
        velocidadGokuX = -5.0;
    }

    // Agregar turbulencia ocasional
    if (QRandomGenerator::global()->bounded(100) < 5) { // 5% de probabilidad
        double turbulencia = QRandomGenerator::global()->bounded(-50, 51) / 100.0; // -0.5 a 0.5
        velocidadGokuY += turbulencia;
    }
}

void MainWindow::aplicarResistenciaAire()
{
    // Aplicar resistencia del aire (fricción)
    velocidadGokuX *= resistenciaAire;
    velocidadGokuY *= resistenciaAire;

    // Eliminar velocidades muy pequeñas para evitar movimientos imperceptibles
    if (qAbs(velocidadGokuX) < 0.01) velocidadGokuX = 0.0;
    if (qAbs(velocidadGokuY) < 0.01) velocidadGokuY = 0.0;
}

void MainWindow::actualizarPosicionGoku()
{
    if (!goku) return;

    QPointF posicionActual = goku->pos();

    // Aplicar velocidades
    posicionActual.setX(posicionActual.x() + velocidadGokuX);
    posicionActual.setY(posicionActual.y() + velocidadGokuY);

    goku->setPos(posicionActual);
}

void MainWindow::verificarLimitesGoku()
{
    if (!goku) return;

    QPointF pos = goku->pos();
    bool posicionCambiada = false;

    // Límites horizontales
    if (pos.x() < 0) {
        pos.setX(0);
        velocidadGokuX = 0; // Detener movimiento al chocar con el borde
        posicionCambiada = true;
    } else if (pos.x() > 1017) { // 1200 - ancho de Goku escalado
        pos.setX(1017);
        velocidadGokuX = 0;
        posicionCambiada = true;
    }

    // Límites verticales
    if (pos.y() < 0) {
        pos.setY(0);
        velocidadGokuY = 0;
        posicionCambiada = true;
    } else if (pos.y() > 489) { // 675 - alto de Goku escalado
        pos.setY(489);
        velocidadGokuY = 0;
        posicionCambiada = true;
    }

    if (posicionCambiada) {
        goku->setPos(pos);
    }
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

    if (vientoActivo && permitirMovimientoVertical) {
        // En nivel 3, los controles modifican la velocidad en lugar de mover directamente
        switch (key) {
        case Qt::Key_W:
            velocidadGokuY -= 2.0; // Impulso hacia arriba
            break;
        case Qt::Key_S:
            velocidadGokuY += 2.0; // Impulso hacia abajo
            break;
        case Qt::Key_A:
            velocidadGokuX -= 3.0; // Impulso hacia la izquierda (contra el viento)
            break;
        case Qt::Key_D:
            velocidadGokuX += 2.0; // Impulso hacia la derecha
            break;
        }

        // Limitar velocidades máximas del jugador
        if (velocidadGokuX < -5.0) velocidadGokuX = -5.0;
        if (velocidadGokuX > 3.0) velocidadGokuX = 3.0;
        if (velocidadGokuY < -3.0) velocidadGokuY = -3.0;
        if (velocidadGokuY > 3.0) velocidadGokuY = 3.0;

    } else {
        // Nivel 2: controles normales
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
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!controlesActivos || !goku) return;

    int key = event->key();
    teclasPresionadas.remove(key);

    // En nivel 3 con físicas, no necesitamos detener el movimiento
    // porque las físicas se encargan del movimiento continuo
    if (!vientoActivo && (key == Qt::Key_A || key == Qt::Key_D)) {
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

void MainWindow::guardarPuntuacionConFecha(int puntuacion)
{
    QString nombreArchivo = "puntuaciones_guardadas.txt";
    QFile archivo(nombreArchivo);

    QString nuevaEntrada = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +
                           " - Puntuación: " + QString::number(puntuacion);

    QStringList lineasExistentes;

    if (archivo.exists()) {
        if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&archivo);
            while (!in.atEnd()) {
                lineasExistentes.append(in.readLine());
            }
            archivo.close();
        } else {
            qDebug() << "No se pudo leer el archivo de puntuaciones.";
            return;
        }
    }

    // Agregar la nueva entrada
    lineasExistentes.append(nuevaEntrada);

    // Escribir todo de nuevo (sobrescribir el archivo completo)
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&archivo);
        for (const QString &linea : lineasExistentes) {
            out << linea << "\n";
        }
        archivo.close();
        qDebug() << "Puntuación guardada correctamente.";
    } else {
        qDebug() << "No se pudo abrir el archivo para escribir.";
    }
}

void MainWindow::iniciarGeneracionBasura(Basura::TipoMovimiento tipo)
{
    detenerGeneracionBasura(); // Detener cualquier generación previa

    timerGenerarBasura = new QTimer(this);
    connect(timerGenerarBasura, &QTimer::timeout, this, &MainWindow::generarBasura);
    timerGenerarBasura->start(2000); // Generar basura cada 2 segundos

    // Inicializar timer para verificar colisiones
    timerColisiones = new QTimer(this);
    connect(timerColisiones, &QTimer::timeout, this, &MainWindow::verificarColisionesPeriodicamente);
    timerColisiones->start(50); // Verificar cada 50ms

    // Solo para nivel 2, iniciar generación de basura4 centralizada
    if (tipo == Basura::Vertical) {
        timerGenerarBasura4 = new QTimer(this);
        connect(timerGenerarBasura4, &QTimer::timeout, this, &MainWindow::generarBasura4);
        timerGenerarBasura4->start(3000); // Generar basura4 cada 3 segundos
    }
}

void MainWindow::generarBasura4()
{
    if (!scene) return;

    // Crear nueva basura4, que estas son realmente esferas q atrapa goku
    Basura *basura4 = new Basura(Basura::Vertical);

    // Configurar posición inicial aleatoria
    int xAleatorio = QRandomGenerator::global()->bounded(10, 1141);
    basura4->setPos(xAleatorio, -50);

    // Configurar como basura4 con movimiento circular
    basura4->configurarComoBasura4();

    // Agregar a la escena y lista
    scene->addItem(basura4);
    listaBasura4.append(basura4);

    // Conectar señal de eliminación
    connect(basura4, &Basura::basuraEliminada, this,
            [this, basura4](bool tocadaPorGoku) {
                eliminarBasura4(basura4, tocadaPorGoku);
            });

    // Iniciar movimiento
    basura4->iniciarMovimiento();

    qDebug() << "Basura4 generada con movimiento circular";
}

void MainWindow::detenerGeneracionBasura()
{
    if (timerGenerarBasura) {
        timerGenerarBasura->stop();
        timerGenerarBasura->deleteLater();
        timerGenerarBasura = nullptr;
    }

    if (timerColisiones) {
        timerColisiones->stop();
        timerColisiones->deleteLater();
        timerColisiones = nullptr;
    }

    // Detener timer de basura4
    if (timerGenerarBasura4) {
        timerGenerarBasura4->stop();
        timerGenerarBasura4->deleteLater();
        timerGenerarBasura4 = nullptr;
    }
}

void MainWindow::generarBasura()
{
    if (!scene) return;

    Basura::TipoMovimiento tipo;

    // Determinar tipo según el nivel actual
    if (vista && vista->scene() == scene) {
        if (permitirMovimientoVertical) {
            tipo = Basura::Horizontal; // Nivel 3
        } else {
            tipo = Basura::Vertical;   // Nivel 2
        }
    } else {
        return; // No estamos en un nivel válido
    }

    Basura *nuevaBasura = new Basura(tipo);
    scene->addItem(nuevaBasura);
    listaBasura.append(nuevaBasura);

    // Conectar señal de eliminación
    connect(nuevaBasura, &Basura::basuraEliminada, this,
            [this, nuevaBasura](bool tocadaPorGoku) {
                eliminarBasura(nuevaBasura, tocadaPorGoku);
            });

    nuevaBasura->iniciarMovimiento();
}

void MainWindow::manejarNuevaBasura4(Basura* basura4)
{
    if (!basura4 || !scene) return;

    // Agregar a la escena
    scene->addItem(basura4);

    // Agregar a la lista de basuras4
    listaBasura4.append(basura4);

    // Conectar señal de eliminación
    connect(basura4, &Basura::basuraEliminada, this,
            [this, basura4](bool tocadaPorGoku) {
                eliminarBasura4(basura4, tocadaPorGoku);
            });

    qDebug() << "Nueva basura4 creada y configurada";
}

void MainWindow::eliminarBasura4(Basura *basura4, bool tocadaPorGoku)
{
    if (!basura4 || !scene) return;

    // Remover de la lista de basuras4
    listaBasura4.removeAll(basura4);

    if (tocadaPorGoku) {
        // En lugar de perder vida, Goku gana 4 puntos y 1 esfera
        puntuacion += 4;
        esferas += 1;
        actualizarTextoPuntaje();
        actualizarTextoEsferas();
        qDebug() << "¡Basura4 recogida! Puntuación actual:" << puntuacion << "Esferas:" << esferas;
    } else {
        // Basura4 salió de límites, pierde 1 punto porque se le fue una esfera
        puntuacion--;
        actualizarTextoPuntaje();
        qDebug() << "Puntuación actual por evitar esfera:" << puntuacion;
    }

    // Remover de la escena y eliminar
    if (basura4->scene()) {
        scene->removeItem(basura4);
    }
    basura4->deleteLater();
}

void MainWindow::eliminarBasura(Basura *basura, bool tocadaPorGoku)
{
    if (!basura || !scene) return;

    // Remover de la lista
    listaBasura.removeAll(basura);

    if (tocadaPorGoku) {
        // Goku perdió una vida
        vida--;
        if((puntuacion-5) < 0){
            puntuacion = 0;
            actualizarTextoPuntaje();
            reproducirSonidoEfecto("qrc:/Audios/VidaMenos.mp3");
        }
        else{
            puntuacion -= 5;
            actualizarTextoPuntaje();
            reproducirSonidoEfecto("qrc:/Audios/VidaMenos.mp3");
        }
        if (indicadorVida) {
            indicadorVida->setVida(vida);
        }

        if (vida <= 0) {
            // Game Over
            if (timerCronometro) {
                timerCronometro->stop();
            }

            // Detener físicas de viento
            detenerFisicasViento();

            // Detener timer del fondo nivel 3 cuando se pierde
            if (timerFondoNivel3) {
                timerFondoNivel3->stop();
            }

            // Restaurar puntuación según el nivel
            if (permitirMovimientoVertical) {
                // Nivel 3: restaurar puntuación del nivel 2
                puntuacion = puntuacionNivel2Ganada;
            } else {
                // Nivel 2: restaurar puntuación del nivel 1
                puntuacion = puntuacionNivel1Ganada;
            }

            vida = 3;
            juegoReiniciado = true;
            QTimer::singleShot(0, this, [this]() {
                limpiarTodo();
                Inicio();
            });
            return;
        }
    } else {
        // Basura salió de límites, ganar 1 punto
        puntuacion++;
        actualizarTextoPuntaje();
        qDebug() << "Puntuación actual por evitar basura:" << puntuacion;
    }

    // Remover de la escena y eliminar
    if (basura->scene()) {
        scene->removeItem(basura);
    }
    basura->deleteLater();
}

void MainWindow::verificarColisionesPeriodicamente()
{
    if (!scene || !goku) return;

    // Verificar colisiones entre Goku y cada basura normal
    for (Basura *basura : listaBasura) {
        if (basura && basura->scene() == scene) {
            if (goku->collidesWithItem(basura)) {
                // Colisión detectada
                eliminarBasura(basura, true);
                return; // Salir del bucle ya que eliminamos una basura
            }
        }
    }

    // Verificar colisiones entre Goku y cada basura4
    for (Basura *basura4 : listaBasura4) {
        if (basura4 && basura4->scene() == scene) {
            if (goku->collidesWithItem(basura4)) {
                // Colisión detectada con basura4
                eliminarBasura4(basura4, true);
                return; // Salir del bucle ya que eliminamos una basura4
            }
        }
    }
}

void MainWindow::limpiarBasura()
{
    // Detener generación de basura
    detenerGeneracionBasura();

    // Limpiar todas las basuras normales de la escena
    for (Basura *basura : listaBasura) {
        if (basura) {
            if (basura->scene()) {
                scene->removeItem(basura);
            }
            basura->deleteLater();
        }
    }
    listaBasura.clear();

    // Limpiar todas las basuras4 de la escena
    for (Basura *basura4 : listaBasura4) {
        if (basura4) {
            if (basura4->scene()) {
                scene->removeItem(basura4);
            }
            basura4->deleteLater();
        }
    }
    listaBasura4.clear();
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
    // Detener timer del fondo nivel 3
    if (timerFondoNivel3) {
        timerFondoNivel3->stop();
    }
    // Detener físicas de viento
    if (timerFisicasViento) {
        timerFisicasViento->stop();
    }

    // Detener audio del nivel
    if (audioNivel) {
        audioNivel->stop();
    }

    if (audioDisparo) {
        audioDisparo->stop();
    }

    // Desconectar todas las señales
    disconnect(this, nullptr, nullptr, nullptr);

    limpiarTodo();
    delete ui;
}
