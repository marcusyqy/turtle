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

  ImNodes::BeginOutputAttribute(3, ImNodesPinShape_Triangle);
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

  // ImNodes::MiniMap();
  ImNodes::EndNodeEditor();

  ImGui::End();
}
