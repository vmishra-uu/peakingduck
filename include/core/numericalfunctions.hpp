////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2019-20, UK Atomic Energy Authority (UKAEA)   //
//                                                                //
////////////////////////////////////////////////////////////////////

/*!
    @file
    Defines the core numerical operations for all data manipulation within of the library.

    @copyright UK Atomic Energy Authority (UKAEA) - 2019-20
*/
#ifndef CORE_NUMERICAL_FUNCTIONS_HPP
#define CORE_NUMERICAL_FUNCTIONS_HPP

#include <algorithm>
#include <vector>

#include "common.hpp"
#include "crtp.hpp"

PEAKINGDUCK_NAMESPACE_START(peakingduck)
PEAKINGDUCK_NAMESPACE_START(core)

    /*!
       @brief To extend the NumericalData type with certain numerical 
       abilities, we add it to this using CRTP to keep it in the interface 
       but it doesn't require us to change the underlying data structure.
    */
    template <class Derived>
    struct NumericalFunctions : public crtp<Derived, NumericalFunctions>
    {
        // using typename Derived::value_type numerical_type;
        
        // standard deviation - not in Eigen but is needed
        decltype(auto) stddev(int ddof=0) const{
            assert(this->underlying().size() > 1);
            return std::sqrt((this->underlying() - this->underlying().mean()).square().sum()/(this->underlying().size()-ddof));
        }

        /*!
            @brief log(log(sqrt(value + 1) + 1) + 1)
            Returns a new array
        */
        Derived LLS() const
        {
            return (((this->underlying() + 1.0).sqrt() + 1.0).log() + 1.0).log();
        }

        /*!
            @brief log(log(sqrt(value + 1) + 1) + 1)
            Changes the underlying array
        */
        Derived& LLSInPlace()
        {
            this->underlying() = this->underlying().LLS();
            return this->underlying();
        }

        /*!
            @brief exp(exp(sqrt(value + 1) + 1) + 1)
            Returns a new array
        */
        Derived inverseLLS() const
        {
            return ((((this->underlying().exp() - 1.0).exp()) - 1.0).square()) - 1.0;
        }

        /*!
            @brief exp(exp(sqrt(value + 1) + 1) + 1)
            Changes the underlying array
        */
        Derived& inverseLLSInPlace()
        {
            this->underlying() = this->underlying().inverseLLS();
            return this->underlying();
        }
        
        /*!
            @brief For each element calculate a new value from the symmetric neigbour values
            at a given order. 
            Take the i-order point and the i+order point and apply a function to that value
            End points are not counted (stay as original) - max(0, i-j) and min(i+j, len(array))

            Returns a new array
        */
        Derived symmetricNeighbourOp(const std::function<void(int, int, const Derived&, Derived&)>& operation, int order=1) const
        {            
            // perform algorithm between these two indices
            const int istart = order;
            const int iend = this->underlying().size() - order;

            // can we do this without copying twice, or even once?
            // since we cannot change in place for each entry in loop as this
            // changes results for other entries later.
            Derived newvalues = this->underlying();
            for(int i=istart; i<iend; ++i){
                operation(i, order, this->underlying(), newvalues);
            }            
            return newvalues;
        }

        /*!
            @brief For each element calculate the numerical gradient value from the adjacent elements at a given 
            order. 
            Take the i-1 point and the i+1 point and determine the grad = (array[i+1]-array[i-1])/2.0.
            End points are handled differently as first point grad = array[1]-array[0]
            End points are handled differently as last point grad = array[-1]-array[-2]

            For example, given an array:      [ 1. ,  2.0,  4.0,  7.0, 11.0, 16.0 ]
            The 1-st order gradient would be: [ 1. ,  1.5,  2.5,  3.5,  4.5,  5.  ]
            The 2-nd order gradient would be: [ 0.5,  0.75, 1.0,  1.0,  0.75, 0.5 ]
            The 3-rd order gradient would be: [ 0.25, 0.25, 0.125,  -0.125,  -0.25, -0.25 ]

            Returns a new array
        */
        Derived gradient(int order=1) const
        {            
            const size_t datasize = this->underlying().size();
            assert(datasize >= 2 && "Cannot compute gradient with less than 2 points.");

            if(order == 0)
                return this->underlying();

            const size_t neighbourDiff = 1;
            auto gradOp = [](int i, int , const Derived& values, Derived& newValues){
                newValues[i] = (values[i+neighbourDiff] - values[i-neighbourDiff])/2.0;
            };

            // this handles everything other than the end points which remain unchanged
            Derived grad = this->underlying().symmetricNeighbourOp(gradOp, neighbourDiff);

            // first and last points
            grad[0] = this->underlying()[1] - this->underlying()[0];
            grad[datasize-1] = this->underlying()[datasize-1] - this->underlying()[datasize-2];

            // recursively compute higher orders
            return grad.gradient(order-1);
        }    

        /*!
            @brief Computes the numerical gradient in place.
             
            See: NumericalFunctions::gradient

            Mutates underlying array
        */
        Derived& gradientInPlace(int order=1)
        {           
            this->underlying() = this->underlying().gradient(order);
            return this->underlying();
        }      

        /*!
            @brief For each element calculate the midpoint value from the adjacent elements at a given 
            order. 
            Take the i-order point and the i+order point and determine the average = (array[i-j]+array[i+j])/2.0.
            End points are not counted (stay as original) - max(0, i-j) and min(i+j, len(array))

            For example, given an array:         [1,   4,   6, 2,   4,   2, 5]
            we have the midpoints for order 0:   [1,   4,   6, 2,   4,   2, 5]
            we have the midpoints for order 1:   [1, 3.5,   3, 5,   2, 4.5, 5]
            we have the midpoints for order 2:   [1,   4, 2.5, 3, 5.5,   2, 5]
            we have the midpoints for order 3:   [1,   4,   6, 3,   4,   2, 5]
            we have the midpoints for order 4+:  [1,   4,   6, 2,   4,   2, 5]

            Returns a new array
        */
        Derived midpoint(int order=1) const
        {            
            auto midpointOp = [](int i, int order, const Derived& values, Derived& newValues){
                newValues[i] = (values[i-order] + values[i+order])/2.0;
            };
            return this->underlying().symmetricNeighbourOp(midpointOp, order);
        }        

        /*!
            @brief For each element calculate the midpoint value from the adjacent elements at a given 
            order. 
             
            See: NumericalFunctions::midpoint

            Mutates underlying array
        */
        Derived& midpointInPlace(int order=1)
        {           
            this->underlying() = this->underlying().midpoint(order);
            return this->underlying();
        }      

        /*!
            @brief Sensitive Nonlinear Iterative Peak (SNIP) algorithm for estimating backgrounds
            ref needed here:

            Allows any form of iterations given an iterator

            Returns a new array
        */
        template<class Iterator>
        Derived snip(Iterator first, Iterator last) const
        { 
            auto midpointMinOp = [](int i, int order, const Derived& values, Derived& newValues){
                newValues[i] = (values[i-order] + values[i+order])/2.0;
                newValues[i] = std::min(newValues[i], values[i]);
            };

            Derived snipped = this->underlying();

            // first scale by LLS
            snipped.LLSInPlace();

            // iterations
            for(auto it=first; it!=last; ++it){
                snipped = snipped.symmetricNeighbourOp(midpointMinOp, *it);
            }

            // lastly scale it back LLS
            snipped.inverseLLSInPlace();

            return snipped;
        }

        /*!
            @brief Sensitive Nonlinear Iterative Peak (SNIP) algorithm for estimating backgrounds
            ref needed here:

            does via increasing window only (ToDo: need to allow decreasing window)

            Deprecate this!

            Returns a new array
        */
        Derived snip(int niterations) const
        { 
            std::vector<int> iterations(niterations);
            std::generate(iterations.begin(), iterations.end(), [n = 1] () mutable { return n++; });
            return this->underlying().snip(iterations.begin(), iterations.end());
        }

        /*!
            @brief Sensitive Nonlinear Iterative Peak (SNIP) algorithm for estimating backgrounds

            See: NumericalFunctions::snip

            Mutates underlying array
        */
        Derived& snipInPlace(int niterations)
        {
            this->underlying() = this->underlying().snip(niterations);
            return this->underlying();
        }

        /*!
            @brief A simple function for filtering values above a certain
            threshold (>=). This is useful to remove entries that are negative 
            for example.

            Returns a new array
        */
        // Derived ramp(const numerical_type& threshold) const
        // {
        //     std::function<numerical_type(numerical_type)> imp = [&](const tnumerical_type& x){
        //         return (x >= threshold) ? x : 0;
        //     };
        //     return this->underlying().unaryExpr(imp);
        // }

        // /*!
        //     @brief A simple function for filtering values above a certain
        //     threshold (>=). This is useful to remove entries that are negative 
        //     for example.

        //     Mutates underlying array
        // */
        // Derived& rampInPlace(const numerical_type& threshold)
        // {
        //     this->underlying() = this->underlying().ramp(threshold);
        //     return this->underlying();
        // }
    };

PEAKINGDUCK_NAMESPACE_END
PEAKINGDUCK_NAMESPACE_END

#endif // CORE_NUMERICAL_FUNCTIONS_HPP