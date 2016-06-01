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
#include <string>
#include <iostream>
#include <cmath>
#include <baxter_core_msgs/EndpointState.h>
#include <baxter_core_msgs/SolvePositionIK.h>
#include <baxter_core_msgs/JointCommand.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>

class ArmController
{

private:
    ros::NodeHandle n;

    ros::Publisher joint_cmd_pub;

    ros::Subscriber endpt_sub;

    ros::ServiceClient ik_client;

    // PoseStamped message to be used as request value for IK solver service
    geometry_msgs::PoseStamped req_pose_stamped;
    geometry_msgs::Pose curr_pose;

    std::string limb;
    int NUM_JOINTS;

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

    std::vector<float> getJointAngles(geometry_msgs::PoseStamped req_pose_stamped);

    /*
     * takes in an array of joint angles and commands the joints to take on the
     * angles specified, moving the arm's position/orientation in the process
     * 
     * @param      a vector of joint angles, in order from shoulder to wrist
     *             
     * @return     N/A
     */

    void publishMoveCommand(std::vector<float> joint_angles);

    /*
     * checks if the arm has completed its intended move by comparing
     * the requested pose and the current pose
     * 
     * @param      N/A
     *             
     * @return     true if the parameters of the current pose is equal to the 
     *             requested pose; false otherwise 
     */

    bool hasMoveCompleted();

    /*
     * checks if two decimal numbers are equal to each other up to two decimal points
     * 
     * @param      two floats x and y
     *             
     * @return     true if they are equal up to 2 decimal points; false otherwise
     */

    bool equalTwoDP(float x, float y);

public:

    ArmController(std::string limb);
    ~ArmController();

    // [NOTE] Style note on naming conventions of functions: OpenCV functions follow
    // a word1Word2 convention as opposed to word1_word2. Given that this is the case,
    // it is better to standardize baxter_tictactoe functions to use the same naming
    // convention.

    // [NOTE] Style note on naming conventions of variables: Given the above, variables
    // should follow a word1_word2 convention for maximum contrast

    void endpointCallback(const baxter_core_msgs::EndpointState& msg);

    void pickUpTile();
    void placeTile();

    /*
     * moves the left arm to a standby position it adopts before and after each of 
     * Baxter's turn
     * 
     * @param      N/A
     * 
     * @return     N/A
     */
    void moveLeftToStandby();

    /*
     * moves the left arm to a standby position it adopts before and after each of 
     * Baxter's turn
     * 
     * @param      N/A
     * 
     * @return     N/A
     */
    void moveRightToRest();
};

