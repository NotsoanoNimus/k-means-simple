/*
 * main.c
 *
 * Main executable for feeding inputs into the K-Means Clustering algorithm.
 */

#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kmeans.h"


/* Two-dimensional input data type as points. */
typedef struct point
{
    double x;
    double y;
} point;


/* Gets the linear distance between two point structures. */
static
double
point_distance(const object left,
               const object right)
{
    point *p_left  = (point *)left;
    point *p_right = (point *)right;

    double delta_x = (p_left->x - p_right->x);
    double delta_y = (p_left->y - p_right->y);

    return (delta_x * delta_x) + (delta_y * delta_y);
}


/* Get and set the new centroid location for a given cluster ID. */
static
void
calculate_centroid(kmeans_meta *self,
                   const int cluster)
{
    /* No objects? Don't do anything. */
    if (self->num_objects <= 0) return;

    /* Local variables. */
    point sum = { 0.0, 0.0 };
    int num_in_cluster = 0;

    /* Iterate all objects in the input data set. */
    for (int i = 0; i < self->num_objects; ++i) {
        /* Only care about objects associated with this cluster's centroid. */
        if (self->cluster_assignments[i] != cluster) continue;

        /* Compute the sums of each dimension. */
        sum.x += ((((point **)self->input_objects)[i])->x);
        sum.y += ((((point **)self->input_objects)[i])->y);

        /* Track how many points ended up in this cluster. */
        ++num_in_cluster;
    }

    /* If any points were present in the cluster, get an average in each dimension. */
    if (num_in_cluster) {
        sum.x /= num_in_cluster;
        sum.y /= num_in_cluster;

        /* Copy the sum result as the new centroid for this cluster ID. */
        memcpy(self->centroids[cluster], &sum, sizeof(point));
    }
}


/* Main entrypoint. */
int
main()
{
    /* Choose different values here to control the experiment. */
    int k = 13;   /* Amount of clusters. */
    int spread = 10;   /* How distant or wide data points should be initially spread. */
    int points_per_cluster = 180000;   /* How many points are generated per cluster. */

    /* Other local variables. */
    unsigned long start_time, duration;
    kmeans_result result;

    /* Wire up the meta structure to track details about the K-Means computation. */
    kmeans_meta m_point = {
            .num_centroids = k,
            .num_objects = k * points_per_cluster,
            .iterations = 1000,   /* Set a maximum amount of iterations for convergence. */
            .get_centroid = calculate_centroid,
            .linear_distance = point_distance,
    };

    /* Allocate heap space for other types where necessary. */
    m_point.input_objects = calloc(m_point.num_objects, sizeof(object));
    m_point.cluster_assignments = calloc(m_point.num_objects, sizeof(int));
    m_point.centroids = calloc(m_point.num_centroids, sizeof(object));

    point *pts = calloc(m_point.num_objects, sizeof(point));

    /* Seed the random number generator with the current time. */
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
            double z2 = spread * i + sqrt(-2 * log2(u1)) * sin(2 * 3.14159265 * u2);

            int n = (i * points_per_cluster) + j;
            pts[n].x = z1;
            pts[n].y = z2;
            m_point.input_objects[n] = &(pts[n]);
        }
    }

    /* DEMO: Uncomment this for a more random point distribution. */
    for (int i = 0; i < m_point.num_centroids; ++i) {
        for (int j = 0; j < points_per_cluster; ++j) {
            int n = (i * points_per_cluster) + j;
            pts[n].x = spread * 1.0 * rand() / RAND_MAX;
            pts[n].y = spread * 1.0 * rand() / RAND_MAX;
            m_point.input_objects[n] = &(pts[n]);
        }
    }

    /* Populate initial centroids with random start positions. */
    printf("-- OK\nInitializing %zu random centroids.\n",
           m_point.num_centroids);
    for (int i = 0; i < m_point.num_centroids; ++i) {
        int r = lround(m_point.num_objects * (1.0 * rand() / RAND_MAX));
        m_point.centroids[i] = m_point.input_objects[r];
        printf("centroid[%d]\t%f\t%f\n",
               i,
               ((point *)(m_point.centroids[i]))->x,
               ((point *)(m_point.centroids[i]))->y);
    }

    /* Start the computation and track its duration. */
    printf("-- OK\nRunning K-means computation...\n");
    start_time = time(NULL);
    result = compute_kmeans(&m_point);
    duration = (time(NULL) - start_time);

    /* Output some runtime details. */
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

    /* Finally, print the full results in a comma-separated value (CSV) format. */
    printf("X, Y, Cluster\n");
    for (int i = 0; i < m_point.num_objects; ++i) {
        point *p = (point *)(m_point.input_objects[i]);

        if (p)
            printf("%f, %f, %d\n", p->x, p->y, m_point.cluster_assignments[i]);
        else
            printf("X, X, %d\n", m_point.cluster_assignments[i]);
    }

    /* Free allocated heap space. Not necessary (since this is the end of main), but clean. */
    free(m_point.input_objects);
    free(m_point.centroids);
    free(m_point.cluster_assignments);
    free(pts);

    return 0;
}
