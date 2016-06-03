/*
 * ArmController
 * ------------------------------
 * 
 * ArmController handles all four types of arm movements during the game:
 * 1) the right arm moving away from the camera's point of view
 * to avoid blocking the board detection nodes, 
 * 2) the left arm moving to a standby position between each turn,
 * 3) the left arm moving to pick up a token, and 
 * 4) the left arm moving to place a token on a board cell
 * 
 */ 

#include <ros/ros.h>
#include <ros/console.h>
// Standard libraries
#include <string>
#include <iostream>
#include <cmath>
// Image-handling libraries
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
// ROS message libraries
#include <baxter_core_msgs/EndpointState.h>
#include <baxter_core_msgs/SolvePositionIK.h>
#include <baxter_core_msgs/JointCommand.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>
#include <sensor_msgs/Range.h>

#include "vacuum_gripper/vacuum_gripper.h"

enum goalType {POSE, COLLISION};

class ArmController
{

private:
    ros::NodeHandle n;

    // publishes joint angles commands in order to move arm
    ros::Publisher joint_cmd_pub;
    // subscribes to end-effector endpoint in order to find current pose
    ros::Subscriber endpt_sub;
    ros::Subscriber ir_sub;
    // requests inverse kinematics service to find joint angles to reach desired pose
    ros::ServiceClient ik_client;

    // 'node handle' for images (e.g img_sub = img_trp.subscribe(...) instead of img_sub = n.subscribe(...))
    image_transport::ImageTransport img_trp;
    // subscribes to hand camera image stream in order to locate tile
    image_transport::Subscriber img_sub;

    // PoseStamped message to be used as request value for IK solver service
    geometry_msgs::PoseStamped req_pose_stamped;
    // Pose message used to store current pose; updated by endpointCallback()
    geometry_msgs::Pose curr_pose;

    float curr_range;
    float curr_max_range;
    float curr_min_range;

    ttt::Vacuum_Gripper * gripper;

    // string indicating whether class instance is meant to control right/left limb
    std::string limb;
    int NUM_JOINTS;
    double CENTER_X;
    double CENTER_Y;
    double CELL_SIDE;
    float IR_RANGE_THRESHOLD;

    /*
     * takes in a position and orientation that the user wants the arm to be in,
     * and uses Baxter's built-in inverse kinematics solver to produce the joint angles
     * at which the right/left joints must have to reach that position and orientation
     * 
     * @param      a PoseStamped specifying the desired end position and orientation,
     *             
     * @return     a vector of joint angles the right/left arm joints should have to reach the desired
     *             position/orientation, in order from shoulder to wrist
     */

    std::vector<float> getJointAngles(geometry_msgs::PoseStamped pose_stamped);

    /*
     * takes in an array of joint angles and commands the joints to take on the
     * angles specified, moving the arm's position/orientation in the process
     * 
     * @param      a vector of joint angles, in order from shoulder to wrist
     *             
     * @return     N/A
     */

    void publishMoveCommand(std::vector<float> joint_angles, goalType goal);

    /*
     * checks if the arm has completed its intended move by comparing
     * the requested pose and the current pose
     * 
     * @param      N/A
     *             
     * @return     true if the parameters of the current pose is equal to the 
     *             requested pose; false otherwise 
     */

    bool hasPoseCompleted();

    bool hasCollided();

    /*
     * checks if two decimal numbers are equal to each other up to two decimal points
     * 
     * @param      two floats x and y
     *             
     * @return     true if they are equal up to 2 decimal points; false otherwise
     */

    bool equalTwoDP(float x, float y);

    /*
     * hover the left arm above the stack of tokens
     * 
     * @param      N/A
     *             
     * @return     N/A
     */

    void hoverAboveTokens();
    void gripToken();

    void hoverAboveBoard();
    void releaseToken(int cell_num);

public:

    ArmController(std::string limb);
    ~ArmController();

    // [NOTE] Style note on naming conventions of functions: OpenCV functions follow
    // a word1Word2 convention as opposed to word1_word2. Given that this is the case,
    // it is better to standardize baxter_tictactoe functions to use the same naming
    // convention.

    // [NOTE] Style note on naming conventions of variables: Given the above, variables
    // should follow a word1_word2 convention for maximum contrast


    /*
     * callback function that sets the current pose to the pose received from 
     * the endpoint state topic
     * 
     * @param      N/A
     * 
     * @return     N/A
     */

    void endpointCallback(const baxter_core_msgs::EndpointState& msg);

    /*
     * image callback function
     * 
     * @param      ImageConstPtr is equal to 'typedef boost::shared_ptr< ::sensor_msgs::Image const>'
     * 
     * @return     N/A
     */

    void imageCallback(const sensor_msgs::ImageConstPtr& msg);

    void IRCallback(const sensor_msgs::RangeConstPtr& msg);

    void pickUpToken();
    void placeToken(int cell_num);

    /*
     * moves the arm to a rest position when not performing a move
     * 
     * @param      N/A
     * 
     * @return     N/A
     */

    void moveToRest();

};

