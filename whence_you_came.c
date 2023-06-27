#include <yed/plugin.h>

typedef yed_frame *frame_ptr_t;
use_tree(frame_ptr_t, str_t);

tree(frame_ptr_t, str_t) map;

static void unload(yed_plugin *self) {
    tree_it(frame_ptr_t, str_t) it;
    tree_traverse(map, it) {
        free(tree_it_val(it));
    }
    tree_free(map);
}

static void whence_you_came(int n_args, char **args) {
    tree_it(frame_ptr_t, str_t)  lookup;
    yed_buffer                  *buff;

    if (n_args != 0) {
        yed_cerr("expected 0 arguments, but got %d", n_args);
        return;
    }

    if (ys->active_frame == NULL) {
        yed_cerr("no active frame");
        return;
    }

    lookup = tree_lookup(map, ys->active_frame);
    if (!tree_it_good(lookup)) {
        yed_cerr("no history for this frame");
        return;
    }

    buff = yed_get_buffer(tree_it_val(lookup));

    yed_frame_set_buff(ys->active_frame, buff);
}

static void register_buff(yed_event *event) {
    const char                  *name;
    tree_it(frame_ptr_t, str_t)  lookup;

    if (event->frame == NULL || event->frame->buffer == NULL) { return; }
    if (event->frame->buffer == event->buffer)                { return; }

    name = event->frame->buffer->name;
    if (name == NULL) { return; }

    lookup = tree_lookup(map, ys->active_frame);
    if (tree_it_good(lookup)) {
        if (strcmp(name, tree_it_val(lookup)) == 0) {
            return;
        }
        free(tree_it_val(lookup));
    }

    tree_insert(map, event->frame, strdup(name));
}

int yed_plugin_boot(yed_plugin *self) {
    yed_event_handler buff_post_load_handler;
    yed_event_handler buff_pre_write_handler;

    YED_PLUG_VERSION_CHECK();

    map = tree_make(frame_ptr_t, str_t);

    yed_plugin_set_command(self, "whence-you-came", whence_you_came);

    buff_post_load_handler.kind = EVENT_BUFFER_PRE_FOCUS;
    buff_post_load_handler.fn   = register_buff;

    yed_plugin_add_event_handler(self, buff_post_load_handler);

    yed_plugin_set_unload_fn(self, unload);

    return 0;
}
