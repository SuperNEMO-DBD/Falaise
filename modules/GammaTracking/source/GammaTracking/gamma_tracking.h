#ifndef GT_GAMMA_TRACKING_H
#define GT_GAMMA_TRACKING_H 1

// Standard library:
#include <map>
#include <list>
#include <cstddef>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>

// This plugin
#include "event.h"

namespace datatools {
  class properties;
}

namespace gt {

  class event;

  //! Implementation of the gamma tracking and combinator
  /*!
   * 2011-2012 Hugon Christophe CENBG
   *
   * The gamma tracking object is a combinator of integers in fonction of
   * doubles.
   * The integers represents PMs or vertexes number, the doubles are probabilities calculations.\n
   * It can give the longest combinaison or the one which has the highest probability.
   * It need an external probability calculator (tof_tools for NAT++) which is
   * able to give the probability between two PMs, and then it calculate the
   * combinaisons with chi square probabilies with (NbPMs-1) degrees of freedom.
   *
   * The most standard usage is :\n

   gamma_tracking gt;
   gt.add_prob(PM1, PM2, prob1);
   gt.add_prob(PM2, PM3, prob2);
   gt.add_prob(PM1, PM3, prob3);
   [...]
   gt.process();
   solution_type gamma_tracked_coll;
   gt.get_reflects(lowest_prob, gamma_tracked_coll);

   *
   */
  class gamma_tracking
  {
  public:

    /// collection of integer, ref for "reference of PM"
    typedef std::list<int> list_type;

    /// 2D collection of integer, ref for "reference of PM"
    typedef std::list<list_type> solution_type;

    /// Default constructor
    gamma_tracking();

    /// Copy constructor
    gamma_tracking(const gamma_tracking&);

    /// Default destructor
    ~gamma_tracking();

    /// Set the logging priority threshold
    void set_logging_priority(datatools::logger::priority priority_);

    /// Return the logging priority threshold
    datatools::logger::priority get_logging_priority() const;

    /// Check the initialization flag
    bool is_initialized() const;

    /// Set the initialization flag
    void set_initialized(bool);

    /// Initialization from parameters
    void initialize(const datatools::properties & config_);

    /// Check if contains calculated tracks
    bool has_tracks();

    /// Just add standalone gamma
    /*!< For complete gamma tracking, gamma_tracking::get_reflect gives also alone gammas*/
    void add(int number1_);

    /// Add 2 ref number combinaison with a proba in [0,1]
    void add_probability(int number1_, int number2_, double proba_);

    /// Add 2 ref number combinaison with a proba in [0,inf]
    void add_chi2(int number1_, int number2_, double chi2_);

    /// Add prestart.
    /*!< It impose starts during combinaison. Faster calculations,
      but less reliable than postarts. \sa gamma_tracking::get_reflects*/
    void add_start(int number_);

    /// Check if an element of gamma_tracking::list_type values_ is in serie collection type
    bool is_inside_serie(const list_type & values_) const;

    /// Check if an element of gamma_tracking::list_type values_ is in gamma_tracking::list_type check_
    bool is_inside(const list_type & check_, const list_type & values_) const;

    /// Check if value_ is in gamma_tracking::list_type check_
    bool is_inside(const list_type & check_, int value_) const;

    /// erase elements of gamma_tracking::list_type values_ is in gamma_tracking::list_type check_
    void extract(list_type & source_, const list_type & values_);

    /// to_ become a unique elements list_type of from_+to_
    void put_inside(const list_type & from_, list_type & to_);

    /// if true, only by prob, else by size and then by prob.
    void set_absolute(bool a_);

    /// check gamma_tracking::set_absolute
    bool is_absolute();

    /// if true, forbid the starts to be elsewhere than at start
    void set_extern(bool e_);

    /// check gamma_tracking::set_extern
    bool is_extern();

    /// Set the minimal probability to continue next combinaisons
    void set_probability_min(double min_prob_);

    /*!<
      \param prob_list_ is the probability for a list of gammas below which
      gamma cluster is excluded
      \param starts_ is the post start list_type. After calculation, the function
      will return only gamma tracked starting with starts_.
      \param exclude_ is the list_type to exclude from the calculations, and so, from the final result.
      \param deathless_starts_ means that the starts can be used in all gt.
      Usefull if starts represents vertexes instead of PM numbers.

      \attribute gamma_tracking::_extern_ allow only gamma tracked with starts, no elsewhere\n
      gamma_tracking::_starts_ will be merged with starts_, and will be taken in care with extern and other

      \return The combinaison of gamma tracked. Each ref will be taken once
      (except for deathless starts), and the longest or the most probable gammas
      tracked will be given in gamma_tracking::ref_col_t format

      \sa gamma_tracking::set_absolute \sa gamma_tracking::set_extern
      \sa gamma_tracking::AddStart \sa gamma_tracking::process

    */
    /// Return the results
    void get_reflects(solution_type & solution_,
                      double prob_list_ = -1.0,
                      const list_type * starts_ = 0,
                      const list_type * exclude_ = 0,
                      bool deathless_starts_ = false);

    /// Return all calculated combination.\sa gamma_tracking::process
    const solution_type & get_all() const;

    /// Get the proba for a gamma tracked
    double get_probability(const list_type & scin_ids_) const;

    /// Get the proba between two ref
    double get_probability(int scin_id1_, int scin_id2_) const;

    /// Get the chi square for a gamma tracked
    double get_chi2(const list_type & scin_ids_) const;

    /// Get the chi square between two ref
    double get_chi2(int scin_id1_, int scin_id2_) const;

    /// Classify the two list_type in order of size and proba. Depend on _absolute_
    static bool sort_reflect(const list_type & ref1_, const list_type & ref2_);

    /// Classify a 2D list_type in order of size and proba. Depend on _absolute_
    void sort_probabilities();

    /// Get the chi square limit of _min_prob_ depend on degree of freedom
    double get_chi_limit(unsigned int);

    /// Get a non mutable reference to internal event data model
    const event & get_event() const;

    /// Get a mutable reference to internal event data model
    event & grab_event();

    /// Prepare process by computing the internal probability of all calorimeter pairs
    void prepare_process() ;

    /// Main calculation before the gamma_tracking::get_reflects
    void process() ;

    /*!< Calculate all of the possible combinaisons of gamma tracked in the
      limit of gamma_tracking::_min_prob_. If there is prestart
      gamma_tracking::_starts_, it does the calculation only for combinaisons which starts with _starts_.
      \sa gamma_tracking::get_reflects \sa gamma_tracking::AddStart*/

    /// Reset the gamma tracking
    void reset() ;

    /// cout information about current gamma tracking (old)
    void dump(std::ostream & out_ = std::clog) const;

    /* virtual void tree_dump (std::ostream & out_         = std::clog,  */
    /* 			const std::string & title_  = "", */
    /* 			const std::string & indent_ = "", */
    /* 			bool inherit_               = false) const{} */
  protected:

    /// Set default attribute value
    void _set_defaults();

  private:

    datatools::logger::priority _logging_priority_; //!< Logging priority threshold
    bool _initialized_;                             //!< Initialization flag
    bool _absolute_;                                //!< Prefer probability rather than size of gamma tracked
    bool _extern_;                                  //!< Impose starts in the gamma tracked, not elsewhere
    int _max_;                                      //!< Maximum size of a gamma tracked
    double _min_prob_;                              //!< Minimal probability to continue the combinating
    list_type _starts_;                             //!< Start collections (pre and post)
    solution_type _serie_;                          //!< The full gamma tracked combinaisons
    std::map<int, double> _min_chi2_;               //!< Dictionnary of chi squares : deg of freedom: size-1 VS the chi2
    std::map<const list_type* ,double> _chi2_;      //!< Dictionnary of chi square based on gamma tracked pointer
    std::map<const list_type* ,double> _proba_;     //!< Dictionnary of probabilities based on gamma tracked pointer
    event _event_;                              //!< Internal gamma tracking event
  };
}

#endif // GT_GAMMA_TRACKING_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
