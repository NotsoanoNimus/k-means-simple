/*
 * multidimensional.c
 *
 * Main executable for running k-means against a multi-dimensional set of inputs.
 */

#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kmeans.h"


/* An 8-dimensional point structure. */
typedef struct hyperpoint
{
    double s;
    double t;
    double u;
    double v;
    double w;
    double x;
    double y;
    double z;
} hyperpoint;


static
double
hyperpoint_distance(const object left,
                    const object right)
{
    hyperpoint *p_left  = (hyperpoint *)left;
    hyperpoint *p_right = (hyperpoint *)right;

    double delta_s = (p_left->s - p_right->s);
    double delta_t = (p_left->t - p_right->t);
    double delta_u = (p_left->u - p_right->u);
    double delta_v = (p_left->v - p_right->v);
    double delta_w = (p_left->w - p_right->w);
    double delta_x = (p_left->x - p_right->x);
    double delta_y = (p_left->y - p_right->y);
    double delta_z = (p_left->z - p_right->z);

    return (delta_s * delta_s) +
           (delta_t * delta_t) +
           (delta_u * delta_u) +
           (delta_v * delta_v) +
           (delta_w * delta_w) +
           (delta_x * delta_x) +
           (delta_y * delta_y) +
           (delta_z * delta_z);
}


static
void
calculate_centroid(kmeans_meta *self,
                   const int cluster)
{
    if (self->num_objects <= 0) return;

    hyperpoint sum;
    memset(&sum, 0, sizeof(hyperpoint));

    int num_in_cluster = 0;

    for (int i = 0; i < self->num_objects; ++i) {
        /* Only care about objects associated with this cluster's centroid. */
        if (self->cluster_assignments[i] != cluster) continue;

        sum.s += ((((hyperpoint **)self->input_objects)[i])->s);
        sum.t += ((((hyperpoint **)self->input_objects)[i])->t);
        sum.u += ((((hyperpoint **)self->input_objects)[i])->u);
        sum.v += ((((hyperpoint **)self->input_objects)[i])->v);
        sum.w += ((((hyperpoint **)self->input_objects)[i])->w);
        sum.x += ((((hyperpoint **)self->input_objects)[i])->x);
        sum.y += ((((hyperpoint **)self->input_objects)[i])->y);
        sum.z += ((((hyperpoint **)self->input_objects)[i])->z);
        ++num_in_cluster;
    }

    if (!num_in_cluster) return;

    sum.s /= num_in_cluster;
    sum.t /= num_in_cluster;
    sum.u /= num_in_cluster;
    sum.v /= num_in_cluster;
    sum.w /= num_in_cluster;
    sum.x /= num_in_cluster;
    sum.y /= num_in_cluster;
    sum.z /= num_in_cluster;

    memcpy(self->centroids[cluster], &sum, sizeof(hyperpoint));
}


int
main()
{
    int k = 5;
    int spread = 20;
    int points_per_cluster = 3;
    unsigned long start_time, duration;
    kmeans_result result;

    kmeans_meta m_point = {
            .num_centroids = k,
            .num_objects = k * points_per_cluster,
            .iterations = 1000,
            .get_centroid = calculate_centroid,
            .linear_distance = hyperpoint_distance,
    };
    m_point.input_objects = calloc(m_point.num_objects, sizeof(object));
    m_point.cluster_assignments = calloc(m_point.num_objects, sizeof(int));
    m_point.centroids = calloc(m_point.num_centroids, sizeof(object));

    hyperpoint *pts = calloc(m_point.num_objects, sizeof(hyperpoint));

    srand(time(NULL));

    /* Initialize groups of points as inputs to the algorithm. */
    printf("Initializing %zu input points.\n", m_point.num_objects);
    for (int i = 0; i < m_point.num_centroids; ++i) {
        for (int j = 0; j < points_per_cluster; ++j) {
            /* This fancy-looking bit keeps this set of points constrained
             * to the area where the cluster will appear, based on spread. */
            double u1 = 1.0 * rand() / RAND_MAX;
            double u2 = 1.0 * rand() / RAND_MAX;

            double z1 = spread * i + sqrt(-2 * log2(u1)) * cos(2 * 3.14159265 * u2);
            double z2 = spread * i + sqrt(-2 * log2(u1)) * cos(2 * 3.14159265 * u2);
            double z3 = spread * i + sqrt(-2 * log2(u1)) * cos(2 * 3.14159265 * u2);
            double z4 = spread * i + sqrt(-2 * log2(u1)) * cos(2 * 3.14159265 * u2);
            double z5 = spread * i + sqrt(-2 * log2(u1)) * cos(2 * 3.14159265 * u2);
            double z6 = spread * i + sqrt(-2 * log2(u1)) * cos(2 * 3.14159265 * u2);
            double z7 = spread * i + sqrt(-2 * log2(u1)) * cos(2 * 3.14159265 * u2);
            double z8 = spread * i + sqrt(-2 * log2(u1)) * sin(2 * 3.14159265 * u2);

            int n = (i * points_per_cluster) + j;
            pts[n].s = z1;
            pts[n].t = z2;
            pts[n].u = z3;
            pts[n].v = z4;
            pts[n].w = z5;
            pts[n].x = z6;
            pts[n].y = z7;
            pts[n].z = z8;

            m_point.input_objects[n] = &(pts[n]);
        }
    }

    /* DEMO: Uncomment this for a more random point distribution. */
    for (int i = 0; i < m_point.num_centroids; ++i) {
        for (int j = 0; j < points_per_cluster; ++j) {
            int n = (i * points_per_cluster) + j;
            pts[n].s = spread * 1.0 * rand() / RAND_MAX;
            pts[n].t = spread * 1.0 * rand() / RAND_MAX;
            pts[n].u = spread * 1.0 * rand() / RAND_MAX;
            pts[n].v = spread * 1.0 * rand() / RAND_MAX;
            pts[n].w = spread * 1.0 * rand() / RAND_MAX;
            pts[n].x = spread * 1.0 * rand() / RAND_MAX;
            pts[n].y = spread * 1.0 * rand() / RAND_MAX;
            pts[n].z = spread * 1.0 * rand() / RAND_MAX;
            m_point.input_objects[n] = &(pts[n]);
        }
    }

    /* Populate initial centroids with random start positions. */
    printf("-- OK\nInitializing %zu random centroids.\n",
           m_point.num_centroids);
    for (int i = 0; i < m_point.num_centroids; ++i) {
        int r = lround(m_point.num_objects * (1.0 * rand() / RAND_MAX));
        m_point.centroids[i] = m_point.input_objects[r];
        printf("centroid[%d]\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
               i,
               ((hyperpoint *)(m_point.centroids[i]))->s,
               ((hyperpoint *)(m_point.centroids[i]))->t,
               ((hyperpoint *)(m_point.centroids[i]))->u,
               ((hyperpoint *)(m_point.centroids[i]))->v,
               ((hyperpoint *)(m_point.centroids[i]))->w,
               ((hyperpoint *)(m_point.centroids[i]))->x,
               ((hyperpoint *)(m_point.centroids[i]))->y,
               ((hyperpoint *)(m_point.centroids[i]))->z);
    }

    printf("-- OK\nRunning K-means computation...\n");
    start_time = time(NULL);
    result = compute_kmeans(&m_point);
    duration = (time(NULL) - start_time);

    printf("-- OK\n\nIteration count: %lu\n       Duration: %lu\n",
           m_point.current_iterations, duration);
    printf("           Pace: %.3f iterations every second\n\n",
           duration > 0
            ? ((1.0 * m_point.current_iterations) / duration)
            : m_point.current_iterations);

    printf("Points per cluster:\n");
    for (int i = 0; i < m_point.num_centroids; ++i) {
        int numpts = 0;
        for (int j = 0; j < m_point.num_objects; ++j)
            if (m_point.cluster_assignments[j] == i) ++numpts;
        printf("\tcentroid[%d]: %d\n", i, numpts);
    }

    /* If the exit code is not OK, stop. */
    if (KMEANS_OK != result) {
        printf("K-Means failed with code: %d\n\n", result);
        return 1;
    }

    /* Finally, print the full results. */
    printf("S, T, U, V, W, X, Y, Z, Cluster\n");
    for (int i = 0; i < m_point.num_objects; ++i) {
        hyperpoint *p = (hyperpoint *)(m_point.input_objects[i]);

        if (p)
            printf("%f, %f, %f, %f, %f, %f, %f, %f, %d\n",
                p->s,
                p->t,
                p->u,
                p->v,
                p->w,
                p->x,
                p->y,
                p->z,
                m_point.cluster_assignments[i]);
        else
            printf("X, X, X, X, X, X, X, X, %d\n", m_point.cluster_assignments[i]);
    }

    free(m_point.input_objects);
    free(m_point.centroids);
    free(m_point.cluster_assignments);
    free(pts);

    return 0;
}
