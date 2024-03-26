#include "logic.hpp"
#include "nodes/imnodes.h"

void node_editor() {

  ImGui::Begin("simple node editor");

  ImNodes::BeginNodeEditor();
  ImNodes::BeginNode(1);

  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted("simple node :)");
  ImNodes::EndNodeTitleBar();

  ImGui::Button("press me");

  ImNodes::BeginInputAttribute(2);
  ImGui::Text("input");
  ImNodes::EndInputAttribute();

  ImNodes::BeginOutputAttribute(3);
  ImGui::Indent(40);
  ImGui::Text("output");
  ImNodes::EndOutputAttribute();

  ImNodes::EndNode();
  ImNodes::EndNodeEditor();

  ImGui::End();
}

mini::Edge<double>& Storage::create_edge(double value) {
  double_pool.emplace_back(std::make_unique<mini::Edge<double>>(value));
  return *double_pool.back();
}

mini::Edge<double>& Storage::add(mini::Edge<double>& first, mini::Edge<double>& second) {
  add_pool.emplace_back(new mini::Node<Add>{ { first, second } });
  auto& node = add_pool.back();
  return std::get<0>(node->edges());
}

mini::Edge<double>& Storage::mul(mini::Edge<double>& first, mini::Edge<double>& second) {
  mul_pool.emplace_back(new mini::Node<Multiply>{ { first, second } });
  auto& node = mul_pool.back();
  return std::get<0>(node->edges());
}
