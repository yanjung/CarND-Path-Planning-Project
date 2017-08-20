#ifndef ESTIMATOR_H_
#define ESTIMATOR_H_
#include <vector>
#include <math.h>
#include <functional>
#include "vehicle.h"


namespace pathplanner {
  using namespace std;

  class Estimator
  {
    public:

      Estimator() {}

      virtual ~Estimator() {}

      double calculate_cost(Vehicle vehicle, vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>>predictions, string state, bool verbose = false);


    private:
      struct collision {
        bool hasCollision = false;
        int step = 1000;
      };

      struct TrajectoryData
      {
        int proposed_lane;
        int current_lane;
        double avg_speed;
        //double max_acceleration;
        //double rms_acceleration;
        double prop_closest_approach;
        double actual_closest_approach;
        collision collides;
      };

      typedef std::function<double(const Estimator&, vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>> predictions, TrajectoryData data)> DelegateType;

      vector<DelegateType> delegates = { (DelegateType)&Estimator::inefficiency_cost,
        //(DelegateType)&Estimator::collision_cost,
        (DelegateType)&Estimator::buffer_cost,
        (DelegateType)&Estimator::change_lane_cost,
        (DelegateType)&Estimator::free_line_cost
      };

      // priority levels for costs
      int const COLLISION = pow(10, 6);
      int const DANGER = pow(10, 5);
      int const COMFORT = pow(10, 3);
      int const EFFICIENCY = pow(10, 3);
      double const MAX_SPEED = 49.5;

      double const DESIRED_BUFFER = 30; // timesteps
      int const PLANNING_HORIZON = 1;

      double const PREDICTION_INTERVAL = 0.5;
      double const INTERVAL = .02;
      double const DISTANCE = 30;
      double const LANE_WIDTH = 4.0;
      double const MIDDLE_LANE = LANE_WIDTH / 2;
      double const MANOEUVRE = 5;
      double const MAX_DISTANCE = 999999;

      double change_lane_cost(vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>> predictions, TrajectoryData data) const;

      double inefficiency_cost(vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>> predictions, TrajectoryData data) const;

      double collision_cost(vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>> predictions, TrajectoryData data) const;

      double buffer_cost(vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>> predictions, TrajectoryData data) const;

      double free_line_cost(vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>> predictions, TrajectoryData data) const;

      TrajectoryData get_helper_data(vector<Vehicle::snapshot> trajectory,
        map<int, vector<Vehicle::prediction>> predictions, string state, int lane);

      bool check_collision(Vehicle::snapshot snap, Vehicle::prediction s_now, string checkstate);

      map<int, vector<Vehicle::prediction>> filter_predictions_by_lane(
        map<int, vector<Vehicle::prediction>> predictions, int lane);
  };

}
#endif //ESTIMATOR_H_