#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include <glib/gtestutils.h>

#include "xml/node-iterators.h"
#include "algorithms/find-if-before.h"

namespace Inkscape {
namespace XML {

/* the id_permitted stuff is a temporary-ish hack */

namespace {

bool id_permitted_internal(GQuark qname) {
    char const *qname_s=g_quark_to_string(qname);
    return !strncmp("svg:", qname_s, 4) || !strncmp("sodipodi:", qname_s, 9) ||
           !strncmp("inkscape:", qname_s, 9);
}


bool id_permitted_internal_memoized(GQuark qname) {
    typedef std::map<GQuark, bool> IdPermittedMap;
    IdPermittedMap id_permitted_names;

    IdPermittedMap::iterator found;
    found = id_permitted_names.find(qname);
    if ( found != id_permitted_names.end() ) {
        return found->second;
    } else {
        bool permitted=id_permitted_internal(qname);
        id_permitted_names[qname] = permitted;
        return permitted;
    }
}

}

bool id_permitted(Node const *node) {
    g_return_val_if_fail(node != NULL, false);

    if ( node->type() != ELEMENT_NODE ) {
        return false;
    }

    return id_permitted_internal_memoized((GQuark)node->code());
}

struct node_matches {
    node_matches(Node const &n) : node(n) {}
    bool operator()(Node const &other) { return &other == &node; }
    Node const &node;
};

/** Returns the sibling before \a node in \a node's parent's children,
 *  or NULL if \a node is the first of those children (or if child is
 *  NULL or has no parent).
 *
 *  Useful in combination with Node::addChild, when you want to insert
 *  a new child _before_ a given existing child.
 *
 *  Note: Involves a linear search (unlike next_node).
 *
 * \pre Links are correct, i.e. \a node isin its parent's children.
 *
 * \post (ret == NULL
 *        ? node == NULL || node->parent() == NULL || node->parent()->firstChild() == node
 *        : ret->next() == node).
 */
Node *previous_node(Node *node) {
    using Inkscape::Algorithms::find_if_before;

    if ( !node || !node->parent() ) {
        return NULL;
    }

    Node *previous=find_if_before<NodeSiblingIterator>(
        node->parent()->firstChild(), NULL, node_matches(*node)
    );

    g_assert(previous == NULL
             ? node->parent()->firstChild() == node
             : previous->next() == node);

    return previous;
}

}
}


/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :
