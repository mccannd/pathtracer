#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <renderthread.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <QTime>

void MyGL::DrawBBox(BVHNode *n, int depth)
{
    if (n == NULL || depth <= 0) return;

    if (n->bounds != NULL) {
        BoundingBox *b = n->bounds;
        prog_flat.draw(*this, *b);
    }

    DrawBBox(n->lesser, depth - 1);
    DrawBBox(n->greater, depth - 1);
}

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent)
{
    setFocusPolicy(Qt::ClickFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    if (intersection_engine.root != NULL) delete intersection_engine.root;
    vao.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    vao.create();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat-color shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();

    //Test scene data initialization
    scene.CreateTestScene();
    integrator.scene = &scene;
    integrator.intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;
    ResizeToSceneCamera();

    for (Geometry *g : scene.objects) {
        if (g->bounds == NULL) g->MakeBoundingBox();
    }
    intersection_engine.root = new BVHNode(scene.objects, 0);
    intersection_engine.root->MakeBounds();
}

void MyGL::resizeGL(int w, int h)
{
    gl_camera = Camera(w, h);

    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the projection matrix
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function updateGL is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the viewproj matrix
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    GLDrawScene();
}

void MyGL::GLDrawScene()
{
    for(Geometry *g : scene.objects)
    {
        if(g->drawMode() == GL_TRIANGLES)
        {
            prog_lambert.setModelMatrix(g->transform.T());
            prog_lambert.draw(*this, *g);
        }
        else if(g->drawMode() == GL_LINES)
        {
            prog_flat.setModelMatrix(g->transform.T());
            prog_flat.draw(*this, *g);
        }
    }
    for(Geometry *l : scene.lights)
    {
        prog_flat.setModelMatrix(l->transform.T());
        prog_flat.draw(*this, *l);
    }
    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(*this, scene.camera);

    if (intersection_engine.root != NULL) {
          DrawBBox(intersection_engine.root, 9);
      }

    //Recursively traverse the BVH hierarchy stored in the intersection engine and draw each node
}

void MyGL::ResizeToSceneCamera()
{
    this->setFixedWidth(scene.camera.width);
    this->setFixedHeight(scene.camera.height);
    gl_camera = Camera(scene.camera);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_F) {
        gl_camera.CopyAttributes(scene.camera);
    } else if (e->key() == Qt::Key_R) {
        scene.camera = Camera(gl_camera);
        scene.camera.recreate();
    }
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::SceneLoadDialog()
{
    QString filepath = QFileDialog::getOpenFileName(0, QString("Load Scene"), QString("../scene_files"), tr("*.xml"));
    if(filepath.length() == 0)
    {
        return;
    }

    QFile file(filepath);
    int i = filepath.length() - 1;
    while(QString::compare(filepath.at(i), QChar('/')) != 0)
    {
        i--;
    }
    QStringRef local_path = filepath.leftRef(i+1);
    //Reset all of our objects
    scene.Clear();
    integrator = Integrator();
    intersection_engine = IntersectionEngine();
    //Load new objects based on the XML file chosen.
    xml_reader.LoadSceneFromFile(file, local_path, scene, integrator);
    integrator.scene = &scene;
    integrator.intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;

    for (Geometry *g : scene.objects) {

        if (g->material->is_cloud) {
            g->SetVoxel();
        }

        if (g->bounds == NULL) g->MakeBoundingBox();
    }

    intersection_engine.root = new BVHNode(scene.objects, 0);
    intersection_engine.root->MakeBounds();
    update();
}

void MyGL::RaytraceScene()
{
    QString filepath = QFileDialog::getSaveFileName(0, QString("Save Image"), QString("../rendered_images"), tr("*.bmp"));
    if(filepath.length() == 0)
    {
        return;
    }

    QTime timer = QTime();
    timer.start();


#define MULTITHREADED
#ifdef MULTITHREADED

//#define DEFAULTTHREAD
#ifdef DEFAULTTHREAD
    //Set up 16 (max) threads
    unsigned int width = scene.camera.width;
    unsigned int height = scene.camera.height;
    unsigned int x_block_size = (width >= 4 ? width/4 : 1);
    unsigned int y_block_size = (height >= 4 ? height/4 : 1);
    unsigned int x_block_count = width > 4 ? width/x_block_size : 1;
    unsigned int y_block_count = height > 4 ? height/y_block_size : 1;
    if(x_block_count * x_block_size < width) x_block_count++;
    if(y_block_count * y_block_size < height) y_block_count++;

    unsigned int num_render_threads = x_block_count * y_block_count;
    RenderThread **render_threads = new RenderThread*[num_render_threads];

    //Launch the render threads we've made
    for(unsigned int Y = 0; Y < y_block_count; Y++)
    {
        //Compute the columns of the image that the thread should render
        unsigned int y_start = Y * y_block_size;
        unsigned int y_end = glm::min((Y + 1) * y_block_size, height);
        for(unsigned int X = 0; X < x_block_count; X++)
        {
            //Compute the rows of the image that the thread should render
            unsigned int x_start = X * x_block_size;
            unsigned int x_end = glm::min((X + 1) * x_block_size, width);
            //Create and run the thread
            render_threads[Y * x_block_count + X] = new RenderThread(x_start, x_end, y_start, y_end, scene.sqrt_samples, 5, &(scene.film), &(scene.camera), &(integrator));
            render_threads[Y * x_block_count + X]->start();
        }
    }

    bool still_running;
    do
    {
        still_running = false;
        for(unsigned int i = 0; i < num_render_threads; i++)
        {
            if(render_threads[i]->isRunning())
            {
                still_running = true;
                break;
            }
        }
        if(still_running)
        {
            //Free the CPU to let the remaining render threads use it
            QThread::yieldCurrentThread();
        }
    }
    while(still_running);

    //Finally, clean up the render thread objects
    for(unsigned int i = 0; i < num_render_threads; i++)
    {
        delete render_threads[i];
    }
    delete [] render_threads;

#else
    //QThreadPool::globalInstance()->setMaxThreadCount(16);
    QThreadPool *rendering_threads = new QThreadPool(this);
    rendering_threads->setMaxThreadCount(24);
    int rowIndex = 0;

//#define ROWS
#ifdef ROWS
    while (rowIndex < scene.camera.height) {
        if (rendering_threads->activeThreadCount() <
                rendering_threads->maxThreadCount()) {



            RenderRunnable *rr = new RenderRunnable(0, scene.camera.width,
                                                rowIndex, rowIndex + 1,
                                                scene.sqrt_samples, 4,
                                                &(scene.film),
                                                &(scene.camera),
                                                &(integrator));
            rr->setAutoDelete(true);
            rendering_threads->start(rr);
            rowIndex++;
        }
    }
#else
    int colIndex = 0;
    // for every row
    while (rowIndex < scene.camera.height) {
        // for every column
        while(colIndex < scene.camera.width) {
            // check if the thread pool is at capacity
            if (rendering_threads->activeThreadCount() <
                    rendering_threads->maxThreadCount()) {

                // create a single thread in the pool and advance to the next pixel

                RenderRunnable *rr = new RenderRunnable(colIndex, colIndex + 1,
                                                    rowIndex, rowIndex + 1,
                                                    scene.sqrt_samples, 4,
                                                    &(scene.film),
                                                    &(scene.camera),
                                                    &(integrator));
                rr->setAutoDelete(true);
                rendering_threads->start(rr);
                colIndex++;
            }
        }
        colIndex = 0;
        rowIndex++;
    }


#endif



    rendering_threads->waitForDone();
    delete rendering_threads;

#endif

#else
    StratifiedPixelSampler pixel_sampler = StratifiedPixelSampler(scene.sqrt_samples, 0);
    for(unsigned int i = 0; i < scene.camera.width; i++)
    {
        for(unsigned int j = 0; j < scene.camera.height; j++)
        {
            integrator.TraceRay(scene.camera.Raycast(200, 170),0);
            //integrator.TraceRay(scene.camera.Raycast(i, j),0);
//            if(i == 110 && j == 133) {
//                float breakpt = 4;

//                QList<glm::vec2> sample_points = pixel_sampler.GetSamples(i, j);
//                glm::vec3 accum_color;
//                for(int a = 0; a < sample_points.size(); a++)
//                {
//                    glm::vec3 color = integrator.TraceRay(scene.camera.Raycast(sample_points[a]), 0);
//                    accum_color += color;
//                }
//                scene.film.pixels[i][j] = accum_color / (float)sample_points.size();
//            }
//            else {
//                scene.film.pixels[i][j] = glm::vec3(1, 0, 0);
//            }
        }
    }
#endif
    int t = timer.elapsed();
    std::cout<<"Final render time: "<< t << "\n";

    // VEACH 100: DEFAULT: 254362
    // VEACH 100: DEBUG ON, 16 THREADS, FULL ROWS: 230107
    // VEACH 100: DEBUG OFF, 16 THREADS, FULL ROWS: 224619
    // VEACH 100: DEBUG OFF, 16 THREADS, HALF ROWS: 222638
    // VEACH 100: DEBUG OFF, 8 THREADS, FULL ROWS: 238515
    // VEACH 100: DEBUG OFF, 24 TRHEADS, FULL ROWS: 218495
    // VEACH 100: DEBUG OFF, 24 THREADS, PIXELS:  213866
    // VEACH 100: DEBUG OFF, 20 THREADS, PIXELS:  214173
    scene.film.WriteImage(filepath);
}
