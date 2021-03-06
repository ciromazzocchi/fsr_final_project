#include "./utility.hpp"
Matrix3d skew(Vector3d v)
{
    Matrix3d s;
    s <<    0,      -v[2],  v[1],
            v[2],   0,      -v[0],
            -v[1],  v[0],   0;
    return s;
}


Vector3d getEta(Eigen::Quaterniond q)
{
  double q0, q1, q2, q3;
  Eigen::Vector3d eta;
  q0 = q.w(); q1 = q.x(); q2 = q.y(); q3 = q.z();
  eta(0) = atan( (q2*q3 + q0*q1) / (pow(q0,2) + pow(q3,2) - 0.5) );
  eta(1) = asin( 2 * (q0*q2 - q1*q3));
  eta(2) = atan( (q1*q2 + q0*q3) / (pow(q0,2) + pow(q1,2) - 0.5) );
  return eta;
}

Matrix3d getR(const Vector3d eta)
{
  Matrix3d R;
  double phi = eta[0]; double th = eta[1]; double psi = eta[2];
  R(0,0) = cos(th)*cos(psi);
  R(0,1) = sin(phi)*sin(th)*cos(psi)-cos(phi)*sin(psi);
  R(0,2) = cos(phi)*sin(th)*cos(psi)+sin(phi)*sin(psi);
  R(1,0) = cos(th)*sin(psi);
  R(1,1) = sin(phi)*sin(th)*sin(psi)+cos(phi)*cos(psi);
  R(1,2) = cos(phi)*sin(th)*sin(psi)-sin(phi)*cos(psi);
  R(2,0) = -sin(th);
  R(2,1) = sin(phi)*cos(th);
  R(2,2) = cos(phi)*cos(th);

  return R;
}

Matrix3d getQ(const Vector3d eta) {
    double phi = eta(0);
    double theta = eta(1);

    Matrix3d Q;
    Q(0,0) = 1; Q(0,1) =          0; Q(0,2) =  -sin(theta);
    Q(1,0) = 0; Q(1,1) =   cos(phi); Q(1,2) =   cos(theta)*sin(phi),
    Q(2,0) = 0; Q(2,1) =  -sin(phi); Q(2,2) =   cos(theta)*cos(phi);

    return Q;
};

Matrix3d getQdot(const Vector3d eta, const Vector3d eta_d) {
    double phi = eta(0);
    double theta = eta(1);
    double phi_d = eta_d(0);
    double theta_d = eta_d(1);
    
    Matrix3d Q_dot;
    Q_dot(0,0) = 0; Q_dot(0,1) =               0; Q_dot(0,2) = -cos(theta)*theta_d;
    Q_dot(1,0) = 0; Q_dot(1,1) = -sin(phi)*phi_d; Q_dot(1,2) = -sin(theta)*sin(phi)*theta_d+cos(theta)*cos(phi)*phi_d;
    Q_dot(2,0) = 0; Q_dot(2,1) = -cos(phi)*phi_d; Q_dot(2,2) = -sin(theta)*cos(phi)*theta_d-cos(theta)*sin(phi)*phi_d;

    return Q_dot;
};

Matrix3d getM(const Vector3d eta, const Matrix3d I)
{
  return getQ(eta).transpose() * I * getQ(eta);
}

Matrix3d getC(const Vector3d eta, const Vector3d eta_d, const Matrix3d I) {
  Eigen::Matrix3d Q = getQ(eta);
  return Q.transpose()*skew(Q*eta_d)*I*Q+Q.transpose()*I*getQdot(eta,eta_d);
}