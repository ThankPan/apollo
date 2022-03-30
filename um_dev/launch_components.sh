source cyber/setup.bash

# Directly launch .dag files

# Transform
nohup mainboard -d modules/transform/dag/static_transform.dag &

# Localization
nohup mainboard -d modules/localization/dag/dag_streaming_rtk_localization.dag &

# Perception
nohup mainboard -d modules/perception/production/dag/dag_streaming_perception.dag &
# Traffic light detection
nohup mainboard -d modules/perception/production/dag/dag_streaming_perception_trafficlights.dag &
# Camera obstacle detection
nohup mainboard -d modules/perception/production/dag/dag_streaming_obstacle_detection.dag &
# Lane detection
nohup mainboard -d modules/perception/production/dag/dag_streaming_perception_lane.dag &
# Motion service
nohup mainboard -d modules/perception/production/dag/dag_motion_service.dag &

# Prediction
nohup mainboard -d modules/prediction/dag/prediction_lego.dag &

# Routing
nohup mainboard -d modules/routing/dag/routing_dag &

# Planning
nohup mainboard -d modules/localization/dag/dag_streaming_rtk_localization.dag &

# Control
nohup mainboard -d modules/localization/dag/dag_streaming_rtk_localization.dag &