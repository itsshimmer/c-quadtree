#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;

// Img cortaImagem(Img* pic) {
//     RGB (*pixels)[pic->width] = (RGB(*)[pic->width]) pic->img;
//     Img newImage;
//     newImage.height = pic->height/2;
//     newImage.width = pic->width/2;
//     RGB newPixels[newImage.height*newImage.width];
//     int newPixelsSize = 0;
//     for(int h = 0; h<newImage.height; h++) {
//         for(int w = 0; w<newImage.width; w++) {
//             newPixels[newPixelsSize] = pixels[h][w];
//             newPixelsSize++;
//         }
//     }
//     newImage.img = newPixels;
    
// }
QuadNode* newNode(int x, int y, int width, int height)
{
    QuadNode* n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}
QuadNode* geraQuadtree(Img* pic, float minDetail, int x, int y, int width, int height)
{
    RGB (*pixels)[pic->width] = (RGB(*)[pic->width]) pic->img;

    // QuadNode* tree = malloc(sizeof(QuadNode));
    // x = x;
    // y = y;
    // width = width;
    // height = height;
    // tree->NW = tree->NE = tree->SW = tree->SE = NULL;
    // tree->color[0] = tree->color[1] = tree->color[2] = 0;
    // tree->id = first++;
    QuadNode* tree = newNode(x,y,width,height);

    int total = 0;
    int totalR = 0;
    int totalG = 0;
    int totalB = 0;

    for(int h = y; h<(y + height); h++) {
        for(int w = x; w<(x + width); w++) {
            totalR = totalR + pixels[h][w].r;
            totalG = totalG + pixels[h][w].g;
            totalB = totalB + pixels[h][w].b;
            total++;
        }
    }
    
    tree->color[0] = totalR/total;
    tree->color[1] = totalG/total;
    tree->color[2] = totalB/total;
    //printf("color[0]: %d, color[1]: %d, color[2]: %d", tree->color[0], tree->color[1], tree->color[2]);

    //calcular diferença 
    total = 0;
    double totalDiff = 0;

    for(int h = y; h<(y + height); h++) {
        for(int w = x; w<(x + width); w++) {
            double R = pow((pixels[h][w].r - tree->color[0]),2);
            double G = pow((pixels[h][w].g - tree->color[1]),2);
            double B = pow((pixels[h][w].b - tree->color[2]),2);
            double diff = sqrt(R+G+B);
            //printf("diff: %f \n", diff);
            totalDiff += diff;
            total++;
        }
    }

    totalDiff = totalDiff/total;
    
    if(totalDiff<minDetail) {
        tree->status = CHEIO;
        return tree;
    } else {
        tree->status = PARCIAL;
        // tree->NW = geraQuadtree(pic, minDetail, x, y, width/2, height/2);
        // tree->NE = geraQuadtree(pic, minDetail, x+(width/2), y, width/2, height/2);
        // tree->SW = geraQuadtree(pic, minDetail, x, (y+height)/2, width/2, height/2);
        // tree->SE = geraQuadtree(pic, minDetail, (x+width)/2, (y+height)/2, width/2, height/2);
        tree->NW = geraQuadtree(pic, minDetail, x, y, width/2, height/2);
        tree->NE = geraQuadtree(pic, minDetail, x+width/2, y, width/2, height/2);
        tree->SW = geraQuadtree(pic, minDetail, x, y+height/2, width/2, height/2);
        tree->SE = geraQuadtree(pic, minDetail, x+width/2, y+height/2, width/2, height/2);
        return tree;
    }

    return tree;
}

    //printf("NivelDet: %f", totalDiff);
    //exit(0);  

// COMENTE a linha abaixo quando seu algoritmo ja estiver funcionando
// Caso contrario, ele ira gerar uma arvore de teste com 3 nodos

// #define DEMO
#ifdef DEMO

    /************************************************************/
    /* Teste: criando uma raiz e dois nodos a mais              */
    /************************************************************/

    QuadNode* raiz = newNode(0,0,width,height);
    raiz->status = PARCIAL;
    raiz->color[0] = 0;
    raiz->color[1] = 0;
    raiz->color[2] = 255;

    QuadNode* nw = newNode(width/2,0,width/2,height/2);
    nw->status = PARCIAL;
    nw->color[0] = 0;
    nw->color[1] = 0;
    nw->color[2] = 255;

    // Aponta da raiz para o nodo nw
    raiz->NW = nw;

    QuadNode* nw2 = newNode(width/2+width/4,0,width/4,height/4);
    nw2->status = CHEIO;
    nw2->color[0] = 255;
    nw2->color[1] = 0;
    nw2->color[2] = 0;

    // Aponta do nodo nw para o nodo nw2
    nw->NW = nw2;

#endif
    // Finalmente, retorna a raiz da árvore

// Limpa a memória ocupada pela árvore
void clearTree(QuadNode* n)
{
    if(n == NULL) return;
    if(n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    //printf("Liberando... %d - %.2f %.2f %.2f %.2f\n", n->status, n->x, n->y, n->width, n->height);
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder() {
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode* raiz) {
    if(raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode* raiz) {
    FILE* fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE* fp, QuadNode* n)
{
    if(n == NULL) return;

    if(n->NE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if(n->NW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if(n->SE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if(n->SW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode* n)
{
    if(n == NULL) return;

    glLineWidth(0.1);

    if(n->status == CHEIO) {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x+n->width-1, n->y);
        glVertex2f(n->x+n->width-1, n->y+n->height-1);
        glVertex2f(n->x, n->y+n->height-1);
        glEnd();
    }

    else if(n->status == PARCIAL)
    {
        if(desenhaBorda) {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x+n->width-1, n->y);
            glVertex2f(n->x+n->width-1, n->y+n->height-1);
            glVertex2f(n->x, n->y+n->height-1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
    // Nodos vazios não precisam ser desenhados... nem armazenados!
}

