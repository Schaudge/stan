#include <stan/services/psis/psis.hpp>
#include <test/unit/services/pathfinder/util.hpp>
#include <gtest/gtest.h>

auto&& blah = stan::math::init_threadpool_tbb(1);

TEST(ServicesPSIS, xl) {
  Eigen::Array<double, -1, 1> test_x(20);
  test_x << 0.00231135747917145, 0.00433831801177895, 0.0108541508266367,
      0.0146361066006147, 0.016979809437058, 0.0175260143161184,
      0.0185788657942587, 0.0221000771645818, 0.0246924617215394,
      0.0274311810928377, 0.0492474466172383, 0.0623614301581945,
      0.0659810585631264, 0.0796802961280512, 0.146305816395337,
      0.184840740340265, 0.20479110080993, 0.257155687804798, 0.414031661251632,
      0.95242504763768;
  Eigen::Array<double, -1, 1> theta(34);
  theta << -141.2015714797, -72.7818997030625, -51.7150259992074,
      -40.5047923559694, -33.2797837849548, -28.1283549132001,
      -24.2170675460217, -21.1168185810503, -18.5812072879574,
      -16.4573241245016, -14.6445871694482, -13.0737624253249,
      -11.6954262460058, -10.4732265260756, -9.37974923155375,
      -8.39387784885848, -7.49904768025677, -6.68205735201866,
      -5.93223884538201, -5.24086486593837, -4.60071729862994,
      -4.00576743436348, -3.45093529145725, -2.93190590437957,
      -2.44498730074187, -1.98699942882981, -1.55518636773351,
      -1.14714626375751, -0.760774912681297, -0.394219954206013,
      -0.045843397405553, 0.285809256288784, 0.602034622751884,
      0.903992807508582;
  auto xx = stan::services::psis::internal::profile_loglikelihood(theta, test_x);
  // std::cout << "\nxx: \n" << xx << "\n";
  Eigen::Array<double, -1, 1> good_vals(34);
  good_vals << 1.06041260401414, 1.15443356538553, 1.19147065735959,
      1.21250394179907, 1.22599729792255, 1.23511270696396, 1.24135988808796,
      1.24556779792965, 1.24823428651338, 1.2496768860712, 1.25010629644014,
      1.24966548304594, 1.24845185767863, 1.24653044098141, 1.24394187312235,
      1.24070727899495, 1.23683106608171, 1.23230223192801, 1.22709445935447,
      1.22116507052987, 1.21445273194457, 1.20687360179519, 1.19831533284665,
      1.18862789869965, 1.17760943289194, 1.16498381246489, 1.15036380398127,
      1.1331873300179, 1.11259974380022, 1.08721662622115, 1.05458546311518,
      1.00973177805282, 0.939927373529039, 0.789031648847585;
  for (Eigen::Index i = 0; i < good_vals.size(); ++i) {
    EXPECT_FLOAT_EQ(xx(i), good_vals(i)) << "Failed for xx(" << std::to_string(i) << ")";
  }
}

TEST(ServicesPSIS, gpdfit) {
  Eigen::Array<double, -1, 1> test_vals(20);
  test_vals << 0.00231135747917145, 0.00433831801177895, 0.0108541508266367,
      0.0146361066006147, 0.016979809437058, 0.0175260143161184,
      0.0185788657942587, 0.0221000771645818, 0.0246924617215394,
      0.0274311810928377, 0.0492474466172383, 0.0623614301581945,
      0.0659810585631264, 0.0796802961280512, 0.146305816395337,
      0.184840740340265, 0.20479110080993, 0.257155687804798, 0.414031661251632,
      0.95242504763768;
  auto xx = stan::services::psis::internal::gpdfit(test_vals);
  EXPECT_FLOAT_EQ(std::get<0>(xx), 0.049593218);
  EXPECT_FLOAT_EQ(std::get<1>(xx), 0.6692217);
}

TEST(ServicesPSIS, psis_smooth_tail) {
  Eigen::Array<double, -1, 1> lw_tail(20);
  lw_tail << -2.99800866573995, -2.95818083027794, -2.83994117100258,
      -2.77722249820924, -2.74024139524875, -2.7318158807467, -2.71577267041072,
      -2.66391328183378, -2.62738195564941, -2.59018538938748,
      -2.33487691705024, -2.20785342121582, -2.17545907545608,
      -2.06156037889818, -1.64051190289183, -1.45922773174626,
      -1.37687465350847, -1.18832703957184, -0.773042236304335, 0;
  double cutoff = -3.04544886711793;
  auto xx = stan::services::psis::internal::psis_smooth_tail(lw_tail, cutoff);
  EXPECT_FLOAT_EQ(std::get<1>(xx), 0.6692217);
  Eigen::Array<double, -1, 1> good_vals(20);
  good_vals << -3.01918021688078, -2.96532035254991, -2.90951591025097,
      -2.85151718673147, -2.79102923559678, -2.72769981748855,
      -2.66110306531575, -2.59071688929131, -2.51589099399911,
      -2.43580038738999, -2.34937567092823, -2.25519460495549,
      -2.15130581516063, -2.03492618209314, -1.90188460651689,
      -1.74550409484869, -1.55406315308333, -1.30389813893124,
      -0.934137936804498, -0.164374913822023;
  auto results = std::get<0>(xx);
  for (Eigen::Index i = 0; i < good_vals.size(); ++i) {
    EXPECT_FLOAT_EQ(results(i), good_vals(i));
  }
}

TEST(ServicesPSIS, get_psis_weights) {
  Eigen::Array<double, -1, 1> lrms(100);
  lrms << 6.34466061445847, 5.4846884595318, 4.88197964898707, 7.81547476520815,
      7.21312186227255, 5.89526945311154, -18.6868826136285, 5.69901858745526,
      6.35360681626944, 5.77318645987978, 8.50016049059785, 5.92318779250444,
      5.82840339724928, 5.09113628071391, 6.12129262536626, 4.31984179270301,
      6.30193161570084, 9.87703514410633, 5.62053686559613, 6.28778119166047,
      7.09981264589709, 5.93732070049026, 6.0505561019043, 6.8315862769884,
      -0.819098494230088, 5.44763560689052, 5.53463585583664, 5.80484176329152,
      5.46857290167872, 5.77073032446458, 6.29678043538326, 6.68916954414988,
      5.33396283695591, 5.73968867099491, 3.25035760636789, 6.6600841019936,
      5.63930951042149, 6.91885431382839, 6.37855252768969, 8.41780741236007,
      7.13679374885758, 8.2365232412145, 5.2835802444081, 6.39628788108502,
      6.59337150242803, 6.1652771323282, 7.03709397310375, 6.7738891963011,
      6.5375357868134, 5.49393374030342, 6.59222199308277, 4.93135267551076,
      6.46612744126477, 6.03842200840931, 7.14521926335963, 4.68850108679013,
      6.07790707896482, 6.70548107233638, 5.88620277715406, 6.34703333111342,
      7.66918172289051, 4.49438078425189, 5.95094024666207, 6.69241777251986,
      5.5101439963624, 2.503448036162, -23.9269498632938, 5.34625598992227,
      7.28684975471885, 5.26909134559616, -12.8992080357359, 0.858354770313134,
      5.91717819403628, 7.54215822705609, 6.25185510839521, 6.49923998951521,
      5.72346470790294, 7.24965318845692, 6.08462071645315, 8.68870810453449,
      6.40363271552548, 6.87902647836638, 6.27944513507825, 5.8631506857814,
      7.70157606865025, 4.94343892730965, 6.11368569361627, 6.22908585289751,
      6.02289466654851, 6.78340253571924, 6.06495986706907, -7.02111971231673,
      4.91652175788515, 5.59472544249795, 7.16126247369561, 6.15854810500356,
      6.62555174418028, 9.103992907802, 6.7399306070758, 6.04794961458687;
  Eigen::Array<double, -1, 1> answer(100);
  answer << 0.00614128190501742, 0.00259883000263199, 0.00142240988349195,
      0.0274531755800624, 0.0157476498780024, 0.00391823907901988,
      8.26414513956511e-14, 0.00322003256443345, 0.00619646954412031,
      0.00346793510111242, 0.0444046803091539, 0.004029171121401,
      0.00366480923122832, 0.00175331655026224, 0.00491192297921304,
      0.000810758279198907, 0.00588439834963157, 0.17822256163288,
      0.00297698115204899, 0.00580171797853455, 0.0121325368865641,
      0.00408651931997244, 0.00457647470707981, 0.00999374265076167,
      4.75420585761083e-06, 0.00250429809109908, 0.00273193115270838,
      0.00357946965550952, 0.00255728407443698, 0.00345942783466891,
      0.00585416468904412, 0.00866717090821407, 0.00223521120433322,
      0.00335369108675144, 0.000278240478565439, 0.00841871317684748,
      0.00303339482203412, 0.0108274835712062, 0.00635298901334288,
      0.0366679814601552, 0.0128890587651439, 0.0313596935307308,
      0.00212538535536358, 0.00646666659496334, 0.00787540323350999,
      0.00513279331867044, 0.0114488824095472, 0.00943345186908113,
      0.00744772541269413, 0.00262296832653951, 0.00786635558506987,
      0.00149440115404536, 0.00693444017895142, 0.00452127888795521,
      0.0137317162483214, 0.001172190208605, 0.00470337325067493,
      0.00880970502513788, 0.00388287423800577, 0.00615587072760303,
      0.0220257764512452, 0.000965367587466615, 0.00414255659344651,
      0.00869536963179888, 0.00266583380629748, 0.000131838303785288,
      4.379914619164e-16, 0.00226285858615159, 0.0183863334607642,
      0.00209481287761514, 2.69620598333185e-11, 2.54440757139945e-05,
      0.00400503003258219, 0.0200460546415302, 0.00559698462905835,
      0.00716790107562062, 0.00329971992325907, 0.0169711871963573,
      0.00473505622866302, 0.057026147481862, 0.00651433804563218,
      0.0102597432232274, 0.00575355555045384, 0.00379438966189875,
      0.0244370937648389, 0.00151257245292768, 0.00487470007189976,
      0.0054709853543924, 0.0044516176713708, 0.00952362373631346,
      0.00464287019866733, 9.6288493325318e-09, 0.00147240135547797,
      0.00290112423098392, 0.0146773422920254, 0.00509837055791414,
      0.00813295746634018, 0.082538932616576, 0.00911848336266506,
      0.00456456171607177;
  loggy warner;
  auto blah = stan::services::psis::psis_weights(lrms, 20, warner);
  for (Eigen::Index i = 0; i < answer.size(); ++i) {
    EXPECT_FLOAT_EQ(blah(i), answer(i));
  }
}



TEST(ServicesPSIS, max_n_elements) {
  Eigen::Array<double, -1, 1> unsorted(21);
  for (Eigen::Index i = 0; i < 21; ++i) {
    unsorted(i) = i;
  }
  auto sorted_tuple = stan::services::psis::internal::largest_n_elements(unsorted, 5);
  Eigen::IOFormat CommaInitFmt(Eigen::FullPrecision, 0, ", ", ", ", "\n",
                               "", "", " ");
  auto sorted_result = std::get<0>(sorted_tuple);
  auto sorted_result_pos = std::get<1>(sorted_tuple);
  //std::cout << "nums:\n" << sorted_result.format(CommaInitFmt) << "\n";
  //std::cout << "position: \n" << sorted_result_pos.format(CommaInitFmt) << "\n";

  Eigen::Array<double, -1, 1> sorted_ans(5);
  sorted_ans << 16, 17, 18, 19, 20;
  Eigen::Array<Eigen::Index, -1, 1> sorted_idx(5);
  sorted_idx << 16, 17, 18, 19, 20;
  for (Eigen::Index i = 0; i < 5; ++i) {
    EXPECT_FLOAT_EQ(sorted_ans(i), sorted_result(i));
    EXPECT_EQ(sorted_idx(i), sorted_result_pos(i));
  }
}
