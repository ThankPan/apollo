// Implement RTKYTLocalizationComponent
// Author: Yuting Xie
// 2022.2.15

#include "modules/localization/rtk_yuting/rtk_yt_component.h"
#include "cyber/time/clock.h"

namespace apollo {
namespace localization {

    RTKYTLocalizationComponent::RTKYTLocalizationComponent()
        : localization_(new RTKYTLocalization()) {}

    bool RTKYTLocalizationComponent::Init() {
        // Set tf broadcaster to create writer for this node
        tf2_broadcaster_.reset(new apollo::transform::TransformBroadcaster(node_));
        if (!InitConfig()) {
            AERROR << "Init Config falseed"
            return false;
        }
        if (!InitIO()) {
            AERROR << "Init Interval falseed"
        }

        return true;
    
    }

    bool RTKYTLocalizationComponent::InitConfig() {
        rtk_config::Config rtk_config; // TODO: (yuting) use own config

        // config_file_path_ is a member of ComponentBase and set in .dag file, Todo: see how it passed
        // This loads one .pb.txt info a proto object
        if (!apollo::cyber::common::GetProtoFromFile(config_file_path_, &rtk_config)) {
            return false;
        }

        AINFO << "RTKYT localization config: " << rtk_config.DebugString();
        localization_topic_ = rtk_config.localization_topic();
        localization_status_topic_ = rtk_config.localization_status_topic();
        imu_topic_ = rtk_config.imu_topic();
        gps_topic_ = rtk_config.gps_topic();
        gps_status_topic_ = rtk_config.gps_status_topic();
        broadcast_tf_frame_id_ = rtk_config.broadcast_tf_frame_id();
        broadcast_tf_child_frame_id_ = rtk_config.broadcast_tf_child_frame_id();

        localization_->InitConfig(rtk_config);

        return true;
    }

    bool RTKYTLocalizationComponent::InitIO() {
        // The readers initialized here wont trigger the process of component, but those defined in .dag will
        corrected_imu_listener_ = node_->CreateReader<CorrectedImu> (
            imu_topic_, std::bind(&RTKYTLocalization::GpsStatusCallback, localization_.get(), std::placeholders::_1));
        ACHECK(corrected_imu_listener_);
        gps_status_listener_ = node_->CreateReader<drivers::gnss::InsStat>(
            gps_status_topic_, std::bind(&RTKLocalization::GpsStatusCallback,
                                   localization_.get(), std::placeholders::_1));
        ACHECK(gps_status_listener_);
        
        // Writers for output channels
        localization_talker_ =
            node_->CreateWriter<LocalizationEstimate>(localization_topic_);
        ACHECK(localization_talker_);
        localization_status_talker_ =
            node_->CreateWriter<LocalizationStatus>(localization_status_topic_);
        ACHECK(localization_status_talker_);
        return true;
    }

    bool RTKYTLocalization::Proc(const std::shared_ptr<Gps>& gps_msg) {
        // Gps message is designated in .dag file, so this would trigger component processing
        // TODO: (yuting) process incoming gps message
        return true;
    }

    void RTKYTLocalizationComponent::PublishPoseBroadcastTF(const LocalizationEstimate& localization) {
        // tf message
        apollo::transform::TransformStamped tf2_msg;

        // Set msg content below
        auto mutable_head = tf2_msg.mutable_header();
        mutable_head->set_timestamp_sec(localization.measurement_time());
        mutable_head->set_frame_id(broadcast_tf_frame_id_);
        tf2_msg.set_child_frame_id(broadcast_tf_child_frame_id_);

        auto mutable_translation = tf2_msg.mutable_transform()->mutable_translation();
        mutable_translation->set_x(localization.pose().position().x());
        mutable_translation->set_y(localization.pose().position().y());
        mutable_translation->set_z(localization.pose().position().z());

        auto mutable_rotation = tf2_msg.mutable_transform()->mutable_rotation();
        mutable_rotation->set_qx(localization.pose().orientation().qx());
        mutable_rotation->set_qy(localization.pose().orientation().qy());
        mutable_rotation->set_qz(localization.pose().orientation().qz());
        mutable_rotation->set_qw(localization.pose().orientation().qw());
    
        // Send it through the broadcaster
        tf2_broadcaster_->SendTransform(tf2_msg);
    }

    void RTKYTLocalizationComponent::PublishPoseBroadcastTopic(const LocalizationEstimate& localization) {
        // Write to output channel once
        localization_talker_->Write(localization);
    }       

    void RTKLocalizationComponent::PublishLocalizationStatus(const LocalizationStatus& localization_status) {
        // Write to output channel once
        localization_status_talker_->Write(localization_status);
    }

} // namespace localization
} // namespace apollo