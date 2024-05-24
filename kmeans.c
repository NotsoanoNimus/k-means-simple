/*
 * kmeans.c
 *
 * Implementation of K-Means Clustering methods.
 */

#include "kmeans.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>


kmeans_result
compute_kmeans(kmeans_meta *meta)
{
    assert(meta);

    assert(meta->get_centroid);
    assert(meta->linear_distance);
    assert(meta->input_objects);
    assert(meta->centroids);
    assert(meta->cluster_assignments);

    assert(meta->num_objects);
    assert(meta->num_centroids);
    assert(meta->num_centroids <= meta->num_objects);

    assert(meta->iterations > 0);

    int iterations = 0;
    int clusters_size = sizeof(int) * meta->num_objects;
    int *clusters_previous = malloc(clusters_size);
    kmeans_result result;

    memset(meta->cluster_assignments, 0, clusters_size);

    while (1) {
        /* Preserve the previous set of cluster assignments. */
        memcpy(clusters_previous, meta->cluster_assignments, clusters_size);

        /* Update the relation of each object to its nearest centroid. */
        for (int i = 0; i < meta->num_objects; ++i) {
            object o = meta->input_objects[i];

            /* Null objects always get assigned a cluster value of -1. */
            if (!o) {
                meta->cluster_assignments[i] = -1;
                continue;
            }

            /* Distance to the first list centroid. */
            double current_distance =
                (meta->linear_distance)(o, meta->centroids[0]);
            int current_cluster = 0;

            /* Check distances to other clusters. */
            for (int cluster = 1; cluster < meta->num_centroids; ++cluster) {
                double distance =
                    (meta->linear_distance)(o, meta->centroids[cluster]);

                /*
                 * If the distance to this centroid is the minimum of the set, then
                 * this cluster is the one the object should associate with.
                 */
                if (distance < current_distance) {
                    current_distance = distance;
                    current_cluster = cluster;
                }
            }

            /* Update which cluster this object belongs to. */
            meta->cluster_assignments[i] = current_cluster;
        }

        /* Update each centroid location. */
        for (int i = 0; i < meta->num_centroids; ++i)
            (meta->get_centroid)(meta, i);

        /*
         * Compare memory. If the previous cluster assignments matches
         * now, then convergence is confirmed.
         */
        if (0 == memcmp(clusters_previous,
                        meta->cluster_assignments,
                        clusters_size)) {
            result = KMEANS_OK;
            goto break_out;
        }

        /* Otherwise, increment the iterations count. */
        if (iterations++ > meta->iterations) {
            result = KMEANS_LIMIT;
            goto break_out;
        }
    }

    /* No matter the result, always perform these actions. */
break_out:
    free(clusters_previous);
    meta->current_iterations = iterations;
    return result;
}