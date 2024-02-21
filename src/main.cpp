#include <class.hpp>
#include <ecs_manager.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <optional>
#include <raylib.h>
#include <raymath.h>

namespace e {
struct Node_t;
struct Shape_t;
} // namespace e

namespace c {
struct Shape_t;

struct Node_t
{
  ECS::Handle_t<e::Shape_t> parent{};
  Vector2                   position{};
};

struct Shape_t
{
  std::vector<ECS::Handle_t<e::Node_t>> nodes{};
  const std::size_t                     max_nodes{ 2 };
};
} // namespace c

namespace e {
struct Node_t : ECS::Class_t<c::Node_t>
{};

struct Shape_t : ECS::Class_t<c::Shape_t>
{};

struct Line_t : ECS::Class_t<Shape_t>
{};

struct Box_t : ECS::Class_t<Shape_t>
{};

struct Circle_t : ECS::Class_t<Shape_t>
{};

struct Curve_t : ECS::Class_t<Shape_t>
{};
} // namespace e

struct ECSConfig_t
{
  using Signatures_t = TMPL::TypeList_t<e::Node_t, e::Shape_t, e::Line_t, e::Box_t, e::Circle_t, e::Curve_t>;
};

using ECSMan_t = ECS::ECSManager_t<ECSConfig_t>;

template<class T>
using is_shape = std::conjunction<ECS::Traits::IsInstanceOf<e::Shape_t, T>, std::negation<std::is_same<T, e::Shape_t>>>;
using Shapes_t = TMPL::Sequence::Filter_t<ECSConfig_t::Signatures_t, is_shape>;

struct Render_t
{
  void draw_shape(auto& line, ECS::Handle_t<e::Line_t>) const
  {
    auto& n1{ ecs_man.GetComponent<c::Node_t>(line.nodes.front()) };
    auto& n2{ ecs_man.GetComponent<c::Node_t>(line.nodes.back()) };
    DrawLineV(n1.position, n2.position, WHITE);
  }

  void draw_shape(auto& circle, ECS::Handle_t<e::Circle_t>) const
  {
    auto& n1{ ecs_man.GetComponent<c::Node_t>(circle.nodes.front()) };
    auto& n2{ ecs_man.GetComponent<c::Node_t>(circle.nodes.back()) };
    DrawLineV(n1.position, n2.position, WHITE);
    DrawCircleLines(n1.position.x, n1.position.y, Vector2Distance(n1.position, n2.position), RAYWHITE);
  }

  void draw_shape(auto& box, ECS::Handle_t<e::Box_t>) const
  {
    auto& n1{ ecs_man.GetComponent<c::Node_t>(box.nodes.front()) };
    auto& n2{ ecs_man.GetComponent<c::Node_t>(box.nodes.back()) };
    auto  w{ n2.position.x - n1.position.x };
    auto  h{ n2.position.y - n1.position.y };
    DrawRectangleLines(n1.position.x, n1.position.y, w, h, RAYWHITE);
  }

  void draw_shape(auto&, auto) const {}

  void update() const
  {
    BeginDrawing();
    ClearBackground(DARKBLUE);
    ecs_man.ForEach<e::Node_t>([&](auto& node, auto) { DrawCircle(node.position.x, node.position.y, 3, RED); });
    TMPL::Sequence::ForEach_t<Shapes_t>::Do(
      [&]<class T>() { ecs_man.ForEach<T>([&](auto& shape, auto e) { draw_shape(shape, e); }); });
    EndDrawing();
  }

  const ECSMan_t& ecs_man{};
};

struct Input_t
{
  void make_shape(std::size_t type_id)
  {
    TMPL::Sequence::ForEach_t<Shapes_t>::Do([&]<class T>() {
      if (type_id == TMPL::Sequence::IndexOf_v<T, Shapes_t>) {
        shape = ecs_man.GetBaseID<e::Shape_t>(ecs_man.CreateEntity<T>());
        node  = get_next_node(*shape);
        node  = get_next_node(*shape);
      }
    });
  }

  std::optional<ECS::Handle_t<e::Node_t>> get_next_node(ECS::Handle_t<e::Shape_t> e)
  {
    auto& s{ ecs_man.GetComponent<c::Shape_t>(*shape) };
    if (s.nodes.size() != s.max_nodes) {
      return { s.nodes.emplace_back(ecs_man.CreateEntity<e::Node_t>(c::Node_t{ e, GetMousePosition() })) };
    }
    return std::nullopt;
  }

  void update()
  {
    if (IsKeyPressed(KEY_L)) {
      shape_id = TMPL::Sequence::IndexOf_v<e::Line_t, Shapes_t>;
    } else if (IsKeyPressed(KEY_C)) {
      shape_id = TMPL::Sequence::IndexOf_v<e::Circle_t, Shapes_t>;
    } else if (IsKeyPressed(KEY_B)) {
      shape_id = TMPL::Sequence::IndexOf_v<e::Box_t, Shapes_t>;
    } else {
      shape_id = std::numeric_limits<std::size_t>::max();
    }
    make_shape(shape_id);
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
      ecs_man.ForEach<e::Node_t>([&](auto& nd, auto e) {
        if (Vector2Distance(GetMousePosition(), nd.position) <= 3.0f) {
          node = e;
        }
      });
    }
    if (node) {
      ecs_man.GetComponent<c::Node_t>(*node).position = GetMousePosition();
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      if (shape) {
        if ((node = get_next_node(*shape)) == std::nullopt) {
          shape.reset();
        }
      } else {
        node.reset();
      }
    }
  }

  ECSMan_t&                                ecs_man;
  std::size_t                              shape_id{ std::numeric_limits<std::size_t>::max() };
  std::optional<ECS::Handle_t<e::Shape_t>> shape{};
  std::optional<ECS::Handle_t<e::Node_t>>  node{};
};

struct Data_t
{
  Input_t&        input_sys;
  const Render_t& render_sys;
};

void
update(void* data)
{
  const Data_t& d{ *reinterpret_cast<const Data_t*>(data) };
  d.input_sys.update();
  d.render_sys.update();
}

auto
main() -> int
{
  ECSMan_t       ecs_man{};
  Input_t        input_sys{ ecs_man };
  const Render_t render_sys{ ecs_man };
  Data_t         data{ input_sys, render_sys };
  InitWindow(800, 800, "CAD");
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(update, &data, 0, 1);
#else
  SetTargetFPS(60);
  while (not WindowShouldClose()) {
    update(&data);
  }
#endif
  CloseWindow();
  return 0;
}
