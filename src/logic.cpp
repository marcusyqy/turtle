#include "logic.hpp"
#include "nodes/imnodes.h"

void node_editor() {
  ImGui::Begin("simple node editor");

  ImNodes::BeginNodeEditor();
  ImNodes::BeginNode(1);

  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted("Can I connect this?");
  ImNodes::EndNodeTitleBar();

  ImGui::Button("press me");

  ImNodes::BeginInputAttribute(2, ImNodesPinShape_Triangle);
  ImGui::Text("input");
  ImNodes::EndInputAttribute();

  ImNodes::BeginOutputAttribute(3, ImNodesPinShape_TriangleFilled);
  ImGui::Indent(40);
  ImGui::Text("output");
  ImNodes::EndOutputAttribute();

  ImNodes::EndNode();

  ImNodes::BeginNode(4);
  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted("Can I connect this lol?");
  ImNodes::EndNodeTitleBar();

  ImGui::Button("press me");

  ImNodes::BeginInputAttribute(5);
  ImGui::Text("input");
  ImNodes::EndInputAttribute();

  ImNodes::BeginOutputAttribute(6);
  //ImGui::Indent(40);
  ImGui::Text("output");
  ImNodes::EndOutputAttribute();
  // ImNodes::BeginStaticAttribute(16);
  // static float f = 1.0f;
  // ImGui::DragFloat("const char *label", &f);
  // ImNodes::EndStaticAttribute();

  ImNodes::EndNode();
  ImNodes::Link(0, 3,5);

  // ImNodes::MiniMap();
  ImNodes::EndNodeEditor();

  // resolve node editor stuff after.
  int start_attr, end_attr;
  if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
  {
    std::cout << "linked " << start_attr << ", " << end_attr << std::endl;
  }

  int node_id;
  if (ImNodes::IsNodeHovered(&node_id))
  {
    std::cout << "hovered " << node_id << std::endl;
  }

  // Note that since many nodes can be selected at once, we first need to query the number of
  // selected nodes before getting them.
  const int num_selected_nodes = ImNodes::NumSelectedNodes();
  if (num_selected_nodes > 0)
  {
    static std::vector<int> selected_nodes;
    selected_nodes.resize(num_selected_nodes);
    ImNodes::GetSelectedNodes(selected_nodes.data());
    for(auto x : selected_nodes) {
      std::cout << "selected " << x << std::endl;
    }
  }

  ImGui::End();
}
