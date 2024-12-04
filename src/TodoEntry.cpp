#include <TodoEntry.hpp>

json TodoEntry::to_json() const { return {{"id", id}, {"title", title}, {"completed", completed}}; }
