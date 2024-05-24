/*
 * kmeans.h
 *
 * Header file for related K-Means Clustering data structures and definitions.
 */

#ifndef CIS579_TERMPROJECT_KMEANS_H
#define CIS579_TERMPROJECT_KMEANS_H

#include <stdlib.h>

/* These expressions are for meta-information about function parameters. */
#define IN
#define OUT


/* A simple 'results' enum. */
typedef enum
{
    KMEANS_LIMIT = -4,
    KMEANS_MALFORMED_INPUT,
    KMEANS_BAD_LENGTH,
    KMEANS_NO_DATA,
    KMEANS_OK = 0
} kmeans_result;


/* Early, prototypical definition of this type. */
typedef struct _kmeans_meta kmeans_meta;


/* Primary method for computing K-Means Clustering from an input parcel. */
kmeans_result
compute_kmeans(
    kmeans_meta *meta IN OUT
);


/* A void pointer represents a generic object type. */
typedef void * object;

/* Prototypical method for calculating centroids. */
typedef void (*func_centroid_t) (
    kmeans_meta  *self    IN OUT,   /* reference of meta holding the method */
    const int     cluster IN   /* the cluster number being modified */
);

/* Prototypical method for computing linear distance between two objects. */
typedef double (*func_linear_distance_t) (
    const object left  IN,   /* Point A */
    const object right IN   /* Point B */
);


/* A meta-structure containing information about the running algorithm. */
struct _kmeans_meta
{
    /* Get the centroid for any collection of objects. */
    func_centroid_t get_centroid;

    /* Get the linear distance between any two objects. */
    func_linear_distance_t linear_distance;

    /*
     * An input array of object pointers. The user is responsible
     * for allocating and freeing this. Any null pointers are grouped
     * separately from the rest of the input objects.
     */
    object *input_objects;

    /* The length of the input_objects array. */
    size_t num_objects;

    /*
     * An array of mutable centroids. There are different methods to
     * initialize this, but the best choice is to use something that
     * already represents a reasonable set, lest the algorithm be skewed.
     * User is responsible for the memory management of this array.
     */
    object *centroids;

    /* The length of the centroids array, AKA 'k'. */
    size_t num_centroids;

    /* How many times the algorithm should run to check convergence. */
    unsigned long iterations;

    /* Current iterations counter. */
    unsigned long current_iterations;

    /* Array to fill with cluster as assigned to objects. User responsible. */
    int * cluster_assignments;
};


#endif   /* CIS579_TERMPROJECT_KMEANS_H */
