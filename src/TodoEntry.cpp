#include "YoshimotoShizuka/TodoEntry.hpp"

json TodoEntry::to_json() const {
    return {{"id", id},
            {"title", title},
            {"completed", completed},
            {"created_at", created_at},
            {"updated_at", updated_at}};
}
