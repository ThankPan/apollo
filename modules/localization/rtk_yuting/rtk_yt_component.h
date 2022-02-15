// The component wrapper for rtk_yt_localization
// Author: Yuting Xie
// 2022.2.14

#ifndef RTK_YT_COMPONENT_H
#define RTK_YT_COMPONENT_H

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/cyber.h"
#include "cyber/message/raw_message.h"

#include "modules/drivers/gnss/proto/ins.pb.h"
#include "modules/localization/proto/gps.pb.h"
#include "modules/localization/proto/imu.pb.h"
#include "modules/localization/proto/localization.pb.h"
#include "modules/localization/proto/rtk_config.pb.h"
#include "modules/localization/rtk/rtk_localization.h"
#include "modules/transform/transform_broadcaster.h"

namespace apollo {
namespace localization {

class RTKYTLocalizationComponent final : public cyber::Component<Gps> {
public:
    RTKYTLocalizationComponent();
    ~RTKYTLocalizationComponent() = default;

    bool Init() override; // overrides Init() in ComponentBase
    bool Proc(const std::shared_ptr<Gps> &gps_msg) override; // overrides Proc() in Component template

private:
    bool InitConfig();
    bool InitIO();

    void PublishPoseBroadcastTF(const LocalizationEstimate &localization);
    void PublishPoseBroadcastTopic(const LocalizationEstimate &localization);
    void PublishLocalizationStatus(const LocalizationStatus &localization_status);

private:
    // Below are component-defined channel w/r, which are not mentioned in .dag
    std::shared_ptr<cyber::Reader<localization::CorrectedImu>>
        corrected_imu_listener_ = nullptr;
    std::shared_ptr<cyber::Reader<drivers::gnss::InsStat>> gps_status_listener_ =
        nullptr;
    std::shared_ptr<cyber::Writer<LocalizationEstimate>> localization_talker_ =
        nullptr;
    std::shared_ptr<cyber::Writer<LocalizationStatus>>
        localization_status_talker_ = nullptr;

    // Topic names are default empty, awaiting configuration
    std::string localization_topic_ = "";
    std::string localization_status_topic_ = "";
    std::string gps_topic_ = "";
    std::string gps_status_topic_ = "";
    std::string imu_topic_ = "";

    // These are the frame id of coord transform to broadcast 
    std::string broadcast_tf_frame_id_ = "";
    std::string broadcast_tf_child_frame_id_ = "";

    // Broadcaster class which is a writer for apollo/tf, see class implementation 
    std::unique_ptr<apollo::transform::TransformBroadcaster> tf2_broadcaster_;
    
    // The actual worker for this component
    std::unique_ptr<RTKYTLocalization> localization_;
};

} // namespace localization
} // namespace apollo

// Dont forget to register the component in cyber
CYBER_REGISTER_COMPONENT(RTKYTLocalizationComponent);

#endif