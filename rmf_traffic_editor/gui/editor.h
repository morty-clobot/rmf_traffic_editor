/*
 * Copyright (C) 2019-2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef EDITOR_H
#define EDITOR_H

#include <map>
#include <string>
#include <vector>

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QSettings>
#include <QUndoStack>

#include "actions/add_edge.h"
#include "actions/move_feature.h"
#include "actions/move_fiducial.h"
#include "actions/move_model.h"
#include "actions/move_vertex.h"
#include "actions/rotate_model.h"
#include "building.h"
#include "editor_model.h"
#include "rendering_options.h"

#include "crowd_sim/crowd_sim_editor_table.h"

class BuildingTable;
class LayerTable;
class LevelTable;
class MapView;
class Level;
class LiftTable;
class TrafficTable;
class CrowdSimTable;

QT_BEGIN_NAMESPACE
class QAction;
class QButtonGroup;
class QComboBox;
class QGraphicsView;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QMenu;
class QMouseEvent;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QTabWidget;
class QTimer;
class QToolButton;
QT_END_NAMESPACE


class Editor : public QMainWindow
{
  Q_OBJECT

public:
  Editor();
  ~Editor();

  static Editor* get_instance();

  /// Load a building, replacing the current building being edited
  bool load_building(const QString& filename);

  /// Attempt to load the most recently saved project, just for convenience
  /// when starting the application since often we want to 'resume' editing.
  bool load_previous_building();

  /// Attempt to restore the previous viewport scale and center point
  void restore_previous_viewport();

protected:
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void keyPressEvent(QKeyEvent* event);
  void closeEvent(QCloseEvent* event);
  void showEvent(QShowEvent* event) override;

private:

  QUndoStack undo_stack;

  enum ToolId
  {
    TOOL_SELECT = 1,
    TOOL_ADD_VERTEX,
    TOOL_MOVE,
    TOOL_ADD_LANE,
    TOOL_ADD_WALL,
    TOOL_ADD_MEAS,
    TOOL_ADD_DOOR,
    TOOL_ADD_MODEL,
    TOOL_ROTATE,
    TOOL_ADD_FLOOR,
    TOOL_EDIT_POLYGON,
    TOOL_ADD_ZONE,
    TOOL_ADD_FEATURE,
    TOOL_ADD_CONSTRAINT,
    TOOL_ADD_FIDUCIAL,
    TOOL_ADD_ROI,
    TOOL_ADD_HOLE,
    TOOL_ADD_HUMAN_LANE,
  } tool_id = TOOL_SELECT;

  std::map<ToolId, QAction*> tools;

  /////////////////
  // MENU ACTIONS
  void building_new();
  void building_open();
  bool building_save();
  bool building_export_features();

  bool maybe_save();
  void edit_undo();
  void edit_redo();
  void edit_preferences();
  void edit_building_properties();
  void edit_project_properties();
  void edit_rotate_all_models();
  void edit_optimize_layer_transforms();
  void edit_align_colinear();

  void level_add();
  void level_edit();
  void level_table_update_slot();

  void layer_table_update(const int row_idx);
  void layer_table_update_slot();

  void zoom_reset();
  void view_models();

  void help_about();

  bool is_mouse_event_in_map(QMouseEvent* e, QPointF& p_scene);

  QToolBar* toolbar;
  QToolButton* create_tool_button(
    const ToolId id,
    const QString& icon_filename,
    const QString& tooltip);
  void tool_toggled(int id, bool checked);

  QComboBox* mode_combo_box;

/////////////////////////////
  static Editor* instance;  // there will only be one instance

  Building building;
  RenderingOptions rendering_options;
  int level_idx = 0;  // level that we are currently editing
  int layer_idx = 0;  // currently selected layer
  int clicked_idx = -1;  // point most recently clicked
  int prev_clicked_idx = -1; // Previously clicked ID.
  //int polygon_idx = -1;  // currently selected polygon
  Polygon* selected_polygon = nullptr;
  QUuid clicked_feature_id;

  Level* active_level();
  Layer* active_layer();

  QGraphicsScene* scene = nullptr;
  MapView* map_view = nullptr;

  QAction* view_models_action = nullptr;

  const QString tool_id_to_string(const int id);
  QButtonGroup* tool_button_group = nullptr;

  QTabWidget* right_tab_widget = nullptr;

  QTableWidget* create_tabbed_table();
  void update_tables();

  void populate_layers_table();
  void layers_table_set_row(
    const int row_idx,
    const QString& label,
    const bool checked);
  void layer_edit_button_clicked(const int row_idx);
  void layer_add_button_clicked();

  LevelTable* level_table = nullptr;
  LayerTable* layer_table = nullptr;
  LiftTable* lift_table = nullptr;
  TrafficTable* traffic_table = nullptr;
  CrowdSimEditorTable* crowd_sim_table = nullptr;

  QTableWidget* property_editor = nullptr;
  void update_property_editor();
  void clear_property_editor();
  void populate_property_editor(const Edge& edge);
  void populate_property_editor(const Model& model);
  void populate_property_editor(const Vertex& vertex, const int index);
  void populate_property_editor(const Feature& feature);
  void populate_property_editor(const Fiducial& fiducial);
  void populate_property_editor(const Polygon& polygon);
  void populate_property_editor(const Layer& layer);

  QTableWidgetItem* create_table_item(const QString& str,
    bool editable = false);
  void property_editor_cell_changed(int row, int column);
  void property_editor_set_row(
    const int row_idx,
    const QString& label,
    const QString& value,
    const bool editable = false);
  void property_editor_set_row(
    const int row_idx,
    const QString& label,
    const int& value,
    const bool editable = false);
  void property_editor_set_row(
    const int row_idx,
    const QString& label,
    const double& value,
    const int max_decimal_places = 3,
    const bool editable = false);
  QPushButton* add_param_button, * delete_param_button;
  void add_param_button_clicked();
  void delete_param_button_clicked();
  void clear_current_tool_buffer(); // Necessary for tools like edge drawing that store temporary states

#ifdef HAS_IGNITION_PLUGIN
  QAction* sim_reset_action;
  QAction* sim_play_pause_action;
  void sim_reset();
  void sim_play_pause();
  SimThread sim_thread;
  QTimer* scene_update_timer;
  void scene_update_timer_timeout();
#endif

public:
  void sim_tick();  // called by SimThread

private:

#if defined(HAS_IGNITION_PLUGIN) && defined(HAS_OPENCV)
  QAction* record_start_stop_action;
  bool is_recording = false;
  void record_start_stop();
  void record_frame_to_video();
  cv::VideoWriter* video_writer = nullptr;
#endif

  std::vector<EditorModel> editor_models;
  EditorModel* mouse_motion_editor_model = nullptr;
  void load_model_names();

  bool create_scene();

  const static int ROTATION_INDICATOR_RADIUS = 50;
  QGraphicsLineItem* mouse_motion_line = nullptr;
  QGraphicsEllipseItem* mouse_motion_ellipse = nullptr;
  QGraphicsPixmapItem* mouse_motion_model = nullptr;
  QGraphicsPolygonItem* mouse_motion_polygon = nullptr;

  int mouse_model_idx = -1;
  int mouse_vertex_idx = -1;
  int mouse_feature_idx = -1;
  int mouse_feature_layer_idx = -1;
  int mouse_fiducial_idx = -1;
  std::vector<int> mouse_motion_polygon_vertices;
  //int mouse_motion_polygon_vertex_idx = -1;
  Polygon::EdgeDragPolygon mouse_edge_drag_polygon;

  void draw_mouse_motion_line_item(const double mouse_x, const double mouse_y);
  void remove_mouse_motion_item();

  void number_key_pressed(const int n);

  // mouse handlers for various tools
  enum MouseType
  {
    MOUSE_UNDEFINED = 0,
    MOUSE_PRESS = 1,
    MOUSE_RELEASE = 2,
    MOUSE_MOVE = 3
  };

  void mouse_event(const MouseType t, QMouseEvent* e);

  // helper function to avoid repeating lots of "add edge" code
  void mouse_add_edge(
    const MouseType t,
    QMouseEvent* e,
    const QPointF& p,
    const Edge::Type& edge_type);

  // helper function to avoid repeating lots of "add polygon" code
  void mouse_add_polygon(
    const MouseType t,
    QMouseEvent* e,
    const QPointF& p,
    const Polygon::Type& polygon_type);

  QGraphicsPixmapItem* get_closest_pixmap_item(const QPointF& p);
  double discretize_angle(const double& angle);
  void align_point(const QPointF& start, QPointF& end);

  void mouse_select(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_move(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_rotate(const MouseType t, QMouseEvent* e, const QPointF& p);

  void mouse_add_vertex(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_feature(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_fiducial(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_lane(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_wall(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_meas(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_door(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_model(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_floor(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_hole(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_add_roi(const MouseType t, QMouseEvent* e, const QPointF& p);
  void mouse_edit_polygon(const MouseType t, QMouseEvent* e, const QPointF& p);

  void mouse_add_human_lane(const MouseType t, QMouseEvent* e,
    const QPointF& p);

  void mouse_add_constraint(
    const MouseType t,
    QMouseEvent* e,
    const QPointF& p);

  QPointF previous_mouse_point;

  // For undo related support
  AddEdgeCommand* latest_add_edge = nullptr;
  MoveFeatureCommand* latest_move_feature = nullptr;
  MoveFiducialCommand* latest_move_fiducial = nullptr;
  MoveModelCommand* latest_move_model = nullptr;
  MoveVertexCommand* latest_move_vertex = nullptr;
  RotateModelCommand* latest_rotate_model = nullptr;

  void sanity_check();
};

#endif
